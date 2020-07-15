#include <vector>
#include <cmath>
#include <limits>
#include <iostream>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

const int width = 800;
const int height = 800;

float *zbuffer;
float *shadowbuffer;

Model *model = NULL;
Vec3f center(0, 0, 0);
Vec3f light_dir = Vec3f(1,1,0).normalize();
Vec3f       eye(1,1,4);
Vec3f        up(0,1,0);


Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

Vec3i vf2i(Vec3f v) {
    return Vec3i(v[0], v[1], v[2]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}


Vec3f projectDivision(Vec4f v) {
    return Vec3f(v[0]/v[3], v[1]/v[3],v[2]/v[3]);
}


int xx = 0;
int yy = 0;

struct ShadowShader{
    Vec4f m2v(Matrix m) {
        return Vec4f(m[0][0], m[1][0], m[2][0],m[3][0]);
    }

    Matrix uniform_M;   // ModelView
    Matrix uniform_MIT; // (Projection*ModelView).inverse().transpose()
    Matrix uniform_Mshadow; // M*(Viewport*Projection*ModelView).inverse()， transform framebuffer screen coordinates to shadowbuffer screen coordinates
    Vec2i varying_uv[3]; // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    Vec3f varying_tri[3]; // triangle coordinates before Viewport transform, written by VS, read by FS

    ShadowShader(Matrix M, Matrix MIT, Matrix MS) : uniform_M(M), uniform_MIT(MIT), uniform_Mshadow(MS), varying_uv(), varying_tri() {}

    Vec4f vertex(int iface, int nthvert) {
        varying_uv[nthvert] = model->uv(iface, nthvert);
        Vec3f vertex = model->vert(iface, nthvert);
        Vec4f gl_vertex(vertex.x, vertex.y, vertex.z, 1);
        gl_vertex = m2v(Viewport * Projection * ModelView * gl_vertex);
        varying_tri[nthvert] = projectDivision(gl_vertex);
        return gl_vertex;
    }

    bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f p = Vec3f(
                varying_tri[0].x * bar.x + varying_tri[1].x * bar.y +varying_tri[2].x * bar.z,
                varying_tri[0].y * bar.x + varying_tri[1].y * bar.y +varying_tri[2].y * bar.z,
                varying_tri[0].z * bar.x + varying_tri[1].z * bar.y +varying_tri[2].z * bar.z
        );
        Vec4f sb_p = m2v(uniform_Mshadow* Vec4f(p.x, p.y, p.z, 1)); // corresponding point in the shadow buffer
        sb_p = sb_p/sb_p[3];
        int idx = int(sb_p[0]) + int(sb_p[1])*width; // index in the shadowbuffer array
        //神奇的z-fighting
        float shadow = .3+.7*(shadowbuffer[idx]<sb_p[2] +43.34);
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        Vec3f normal = model->norm(uv);
        Vec4f nt = m2v(uniform_MIT * Vec4f(normal.x, normal.y, normal.z, 1));
        Vec3f n = Vec3f(nt.x, nt.y, nt.z).normalize();
        //法向量不能随便使用矩阵转换空间，尤其是发生scale的情况下，要使用矩阵的逆矩阵转置矩阵
        //转到世界空间
        Vec4f lt = m2v((uniform_M * Vec4f(light_dir.x, light_dir.y, light_dir.z, 1)));
        Vec3f l =Vec3f(lt.x, lt.y, lt.z).normalize(); // light vector
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n*l);
        TGAColor c = model->diffuse(uv);
        for (int i=0; i<3; i++) color[i] = std::min<float>(20 + c[i]*shadow*(1.2*diff + .6*spec), 255);
        return false;
    }
};

struct DepthShader{
    Vec4f m2v(Matrix m) {
        return Vec4f(m[0][0], m[1][0], m[2][0],m[3][0]);
    }

    Vec3f varying_tri[3];

    Vec4f vertex4f(int iface, int nthvert) {
        Vec3f vertex = model->vert(iface, nthvert);
        Vec4f gl_vertex(vertex.x, vertex.y, vertex.z, 1);
        gl_vertex = m2v(Viewport * Projection * ModelView * gl_vertex);
        varying_tri[nthvert] = projectDivision(gl_vertex);
        return gl_vertex;
    }

    bool fragment(Vec3f bar, TGAColor &color) {
        Vec3f p = Vec3f(
                varying_tri[0].x * bar.x + varying_tri[1].x * bar.y +varying_tri[2].x * bar.z,
                varying_tri[0].y * bar.x + varying_tri[1].y * bar.y +varying_tri[2].y * bar.z,
                varying_tri[0].z * bar.x + varying_tri[1].z * bar.y +varying_tri[2].z * bar.z
        );
        color = TGAColor(255, 255, 255)*(p.z/depth);
        return false;
    }
};

bool on = false;

void triangle_shadow(Vec4f *pts, DepthShader &shader, TGAImage &image, float *zbuffer) {
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
        }
    }
    Vec2i P;
    TGAColor color;

    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec2f a = getV2fromV4(pts[0]/pts[0][3]);
            Vec2f b = getV2fromV4(pts[1]/pts[1][3]);
            Vec2f e = (getV2fromV4(pts[2]/pts[2][3]));
            Vec3f c = barycentric(getV2fromV4(pts[0]/pts[0][3]), getV2fromV4(pts[1]/pts[1][3]), getV2fromV4(pts[2]/pts[2][3]), Vec2f(P.x, P.y));

            float z = pts[0][2]*c.x + pts[1][2]*c.y + pts[2][2]*c.z;
            float w = pts[0][3]*c.x + pts[1][3]*c.y + pts[2][3]*c.z;
            int frag_depth = z/w;
            if (c.x<0 || c.y<0 || c.z<0 || zbuffer[(int)(P.x+P.y*image.get_width())]>frag_depth) continue;
            bool discard = shader.fragment(c, color);
            if (!discard) {
                zbuffer[(int)(P.x+P.y*image.get_width())] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}

void triangle_final(Vec4f *pts, ShadowShader &shader, TGAImage &image, float *zbuffer) {
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
        }
    }


    Vec2i P;
    TGAColor color;

    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec2f a = getV2fromV4(pts[0]/pts[0][3]);
            Vec2f b = getV2fromV4(pts[1]/pts[1][3]);
            Vec2f e = (getV2fromV4(pts[2]/pts[2][3]));
            if (on && xx < 4) {
                std::cerr<<"barycentric#bboxmin = " <<  bboxmin <<std::endl;
                std::cerr<<"barycentric#bboxmax = " <<  bboxmax <<std::endl;

                std::cerr<<"barycentric#0 = " <<  a <<std::endl;
                std::cerr<<"barycentric#1 = " <<  b<<std::endl;
                std::cerr<<"barycentric#2 = " <<  e <<std::endl;
                std::cerr<<"barycentric#3 = " <<  P <<std::endl;
            }
            Vec3f c = barycentric(getV2fromV4(pts[0]/pts[0][3]), getV2fromV4(pts[1]/pts[1][3]), getV2fromV4(pts[2]/pts[2][3]), Vec2f(P.x, P.y));

            if (on && xx < 4) {
                std::cerr<<"barycentric#c = " << c<<std::endl;
                xx ++;
            }

            float z = pts[0][2]*c.x + pts[1][2]*c.y + pts[2][2]*c.z;
            float w = pts[0][3]*c.x + pts[1][3]*c.y + pts[2][3]*c.z;
            int frag_depth = z/w;
            if (c.x<0 || c.y<0 || c.z<0 || zbuffer[(int)(P.x+P.y*image.get_width())]>frag_depth) continue;
            bool discard = shader.fragment(c, color);
            if (!discard) {
                zbuffer[(int)(P.x+P.y*image.get_width())] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}

int main(int argc, char **argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    zbuffer = new float[width*height];
    shadowbuffer = new float[width*height];
    for (int i = 0 ;i < width * height; i++) {
        zbuffer[i] = shadowbuffer[i] = std::numeric_limits<int>().min();
    }

    { // rendering the shadow buffer
        TGAImage depth(width, height, TGAImage::RGB);
        lookat(light_dir, center, up);
        viewport(width/8, height/8, width*3/4, height*3/4);
        projection(0);
        DepthShader depthshader;
        Vec4f screen_coords[3];
        for (int i=0; i<model->nfaces(); i++) {
            for (int j=0; j<3; j++) {
                screen_coords[j] = depthshader.vertex4f(i, j);
                if (i == 0) {
                    std::cerr<<"screen_coords = " << screen_coords[j]<<std::endl;
                }
            }
            triangle_shadow(screen_coords, depthshader, depth, shadowbuffer);
        }
        depth.flip_vertically(); // to place the origin in the bottom left corner of the image
        depth.write_tga_file("lession7_depth.tga");
    }

    Matrix M = Viewport*Projection*ModelView;

    on = true;
    { // rendering the frame buffer
        TGAImage frame(width, height, TGAImage::RGB);
        lookat(eye, center, up);
        viewport(width/8, height/8, width*3/4, height*3/4);
        projection(-1.f/(eye-center).norm());
        ShadowShader shader(ModelView, (Projection*ModelView).inverse().transpose(), M*((Viewport*Projection*ModelView).inverse()));
        Vec4f screen_coords[3];
        for (int i=0; i<model->nfaces(); i++) {
            for (int j=0; j<3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle_final(screen_coords, shader, frame, zbuffer);
        }
        frame.flip_vertically(); // to place the origin in the bottom left corner of the image
        frame.write_tga_file("lession7_final.tga");
    }

    delete model;
    delete [] zbuffer;
    delete [] shadowbuffer;
    return 0;
}


