#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const int width  = 800;
const int height = 800;
const int depth  = 255;

Model *model = NULL;
int *zbuffer = NULL;
Vec3f light_dir = Vec3f(1,-1,1).normalize();
Vec3f eye(1,1,3);
Vec3f center(0,0,0);

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

Matrix viewport(int x, int y, int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (up^z).normalize();
    Vec3f y = (z^x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}

void triangle_texture(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i uv0, Vec2i uv1, Vec2i uv2, TGAImage &image, float ity0, float ity1, float ity2, int *zbuffer) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); std::swap(ity0, ity1);}
    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); std::swap(ity0, ity2); }
    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); std::swap(ity1, ity2); }

    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here

        Vec3i A =               t0 + (t2-t0)*alpha;
        Vec3i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;

        Vec2i uvA =               uv0 +      (uv2-uv0)*alpha;
        Vec2i uvB = second_half ? uv1 +      (uv2-uv1)*beta : uv0 +      (uv1-uv0)*beta;

        float ityA =               ity0 +   (ity2-ity0)*alpha;
        float ityB = second_half ? ity1 +   (ity2-ity1)*beta : ity0 +   (ity1-ity0)*beta;

        if (A.x>B.x) {
            std::swap(A, B);
            std::swap(uvA, uvB);
            std::swap(ityA, ityB);
        }

        for (int j=A.x; j<=B.x; j++) {
            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
            Vec3i P   =   A +       (B-A)*phi;
            Vec2i uvP = uvA +   (uvB-uvA)*phi;
            float ityP =    ityA  + (ityB-ityA)*phi;
            int idx = j+(t0.y+i)*width;
            if (zbuffer[idx]<P.z) {
                zbuffer[idx] = P.z;
                TGAColor color = model->diffuse(uvP);
                if (ityP > 0) {
                    image.set(j, (t0.y + i),
                              TGAColor(color.bgra[2] * ityP, color.bgra[1] * ityP, color.bgra[0] * ityP));
                }
//                image.set(P.x, P.y, TGAColor(color.bgra[2]*intensity, color.bgra[1]*intensity, color.bgra[0]*intensity));
            }
        }
    }
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, TGAImage &image, float ity0, float ity1, float ity2, int *zbuffer) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) { std::swap(t0, t1); std::swap(ity0, ity1); }
    if (t0.y>t2.y) { std::swap(t0, t2); std::swap(ity0, ity2); }
    if (t1.y>t2.y) { std::swap(t1, t2); std::swap(ity1, ity2); }

    int total_height = t2.y-t0.y;

    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec3i A =               t0 + (t2-t0)*alpha;
        Vec3i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;

        float ityA =               ity0 +   (ity2-ity0)*alpha;
        float ityB = second_half ? ity1 +   (ity2-ity1)*beta : ity0 +   (ity1-ity0)*beta;

        if (A.x>B.x) { std::swap(A, B); std::swap(ityA, ityB); }
        for (int j=A.x; j<=B.x; j++) {
            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
            Vec3i P = A + (B-A)*phi;
            float ityP =    ityA  + (ityB-ityA)*phi;
//            std::cout<<"instensity = " << ityP << "\n";
            P.x = j;
            P.y = t0.y+i; // a hack to fill holes (due to int cast precision problems)
            int idx = j+(t0.y+i)*width;
            if (zbuffer[idx]<P.z) {
                zbuffer[idx] = P.z;
                if (ityP > 0) {
                    image.set(j, (t0.y+i), TGAColor(255, 255, 255)*ityP);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    Matrix ModelView  = lookat(eye, center, Vec3f(0,1,0));
    Matrix Projection = Matrix::identity(4);
    Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4);
    Projection[3][2] = -1.f/(eye-center).norm();

    { // draw the model
        int* zbuffer = new int[width*height];
        for (int i=0; i<width*height; i++) {
            zbuffer[i] = std::numeric_limits<int>::min();
        }
        TGAImage image(width, height, TGAImage::RGB);
        for (int i=0; i<model->nfaces(); i++) {
            std::vector<int> face = model->face(i);
            Vec3i screen_coords[3];
            float intensity[3];
            for (int j=0; j<3; j++) {
                Vec3f v = model->vert(face[j]);
                Vec3f tmp = m2v(ViewPort*Projection*ModelView*Matrix(v));
                screen_coords[j] = Vec3i (tmp[0], tmp[1], tmp[2]);
                intensity[j] = model->norm(i, j)*light_dir;
            }
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, intensity[0], intensity[1], intensity[2], zbuffer);
        }

        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("l5_camera.tga");

        delete [] zbuffer;
    }

//    { // dump z-buffer (debugging purposes only)
//        TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
//        for (int i=0; i<width; i++) {
//            for (int j=0; j<height; j++) {
//                zbimage.set(i, j, TGAColor(zbuffer[i+j*width], 1));
//            }
//        }
//        zbimage.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//        zbimage.write_tga_file("zbuffer.tga");
//    }


    //texture
    {
        int* zbuffer = new int[width*height];
        for (int i=0; i<width*height; i++) {
            zbuffer[i] = std::numeric_limits<int>::min();
        }
        TGAImage image(width, height, TGAImage::RGB);
        for (int i=0; i<model->nfaces(); i++) {
            std::vector<int> face = model->face(i);
            Vec3i screen_coords[3];
            float intensity[3];
            for (int j=0; j<3; j++) {
                Vec3f v = model->vert(face[j]);
                Vec3f tmp = m2v(ViewPort * Projection * ModelView * Matrix(v));
                screen_coords[j] = Vec3i (tmp[0], tmp[1], tmp[2]);
//                screen_coords[j] = Vec3i((v.x+1.)*width/2., (v.y+1.)*height/2., (v.z+1.)*depth/2.);
                intensity[j] = model->norm(i, j)*light_dir;
            }
            Vec2i uv[3];
            for (int k=0; k<3; k++) {
                uv[k] = model->uv(i, k);
            }
            triangle_texture(screen_coords[0], screen_coords[1], screen_coords[2], uv[0], uv[1], uv[2], image, intensity[0],intensity[1],intensity[2] , zbuffer);
        }

        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("l5_camera_texture.tga");

        delete [] zbuffer;
    }

    delete model;

    return 0;
}

