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

Model *model = NULL;
Vec3f center(0, 0, 0);
Vec3f       eye(1,1,4);
Vec3f        up(0,1,0);

TGAImage occl(1024, 1024, TGAImage::GRAYSCALE);

Vec4f m2v(Matrix m) {
    return Vec4f(m[0][0], m[1][0], m[2][0], m[3][0]);
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
    return Vec3f(v[0] / v[3], v[1] / v[3], v[2] / v[3]);
}

Vec3f rand_point_on_unit_sphere() {
    float u = (float) rand() / (float) RAND_MAX;
    float v = (float) rand() / (float) RAND_MAX;
    float theta = 2.f * M_PI * u;
    float phi = acos(2.f * v - 1.f);
    return Vec3f(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
}

Vec3f proj3(Vec4f v) {
    return Vec3f(v.x, v.y, v.z);
}

struct InnerShader {
    virtual Vec4f vertex(int iface, int nthvert) = 0;

    virtual bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) = 0;
};

struct DepthShader : public InnerShader {
    Vec3f varying_tri[3];

    Vec4f vertex(int iface, int nthvert) {
        Vec3f vertex = model->vert(iface, nthvert);
        Vec4f gl_vertex(vertex.x, vertex.y, vertex.z, 1);
        gl_vertex = m2v(Viewport * Projection * ModelView * gl_vertex);
        varying_tri[nthvert] = projectDivision(gl_vertex);
        return gl_vertex;
    }

    bool fragment(Vec3f gl_FragCoord, Vec3f bar, TGAColor &color) {
        Vec3f p = Vec3f(
                varying_tri[0].x * bar.x + varying_tri[1].x * bar.y + varying_tri[2].x * bar.z,
                varying_tri[0].y * bar.x + varying_tri[1].y * bar.y + varying_tri[2].y * bar.z,
                varying_tri[0].z * bar.x + varying_tri[1].z * bar.y + varying_tri[2].z * bar.z
        );
        color = TGAColor(255, 255, 255) * (p.z / depth);
//        color = TGAColor(255, 255, 255) * ((gl_FragCoord.z + 1.f) / 2.f);
        return false;
    }
};

void triangle_shadow(Vec4f *pts, InnerShader &shader, TGAImage &image, float *zbuffer) {
    Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j] / pts[i][3]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j] / pts[i][3]);
        }
    }
    Vec2i P;
    TGAColor color;

    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            Vec3f c = barycentric(getV2fromV4(pts[0] / pts[0][3]), getV2fromV4(pts[1] / pts[1][3]),
                                  getV2fromV4(pts[2] / pts[2][3]), Vec2f(P.x, P.y));
            float z = pts[0][2] * c.x + pts[1][2] * c.y + pts[2][2] * c.z;
            float w = pts[0][3] * c.x + pts[1][3] * c.y + pts[2][3] * c.z;
            int frag_depth = z / w;
            if (c.x < 0 || c.y < 0 || c.z < 0) continue;
            if (zbuffer[(int) (P.x + P.y * image.get_width())] > frag_depth) {
                continue;
            }
            bool discard = shader.fragment(Vec3f(P.x, P.y, frag_depth), c, color);
            if (!discard) {
                zbuffer[(int) (P.x + P.y * image.get_width())] = frag_depth;
                image.set(P.x, P.y, color);
            }
        }
    }
}

//void triangle_final(Vec4f *pts, ShadowShader &shader, TGAImage &image, float *zbuffer) {
//    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
//    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
//    for (int i=0; i<3; i++) {
//        for (int j=0; j<2; j++) {
//            bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
//            bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
//        }
//    }
//
//
//    Vec2i P;
//    TGAColor color;
//
//    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
//        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
//            Vec2f a = getV2fromV4(pts[0]/pts[0][3]);
//            Vec2f b = getV2fromV4(pts[1]/pts[1][3]);
//            Vec2f e = (getV2fromV4(pts[2]/pts[2][3]));
//
//            Vec3f c = barycentric(getV2fromV4(pts[0]/pts[0][3]), getV2fromV4(pts[1]/pts[1][3]), getV2fromV4(pts[2]/pts[2][3]), Vec2f(P.x, P.y));
//
//            float z = pts[0][2]*c.x + pts[1][2]*c.y + pts[2][2]*c.z;
//            float w = pts[0][3]*c.x + pts[1][3]*c.y + pts[2][3]*c.z;
//            int frag_depth = z/w;
//            if (c.x<0 || c.y<0 || c.z<0 || zbuffer[(int)(P.x+P.y*image.get_width())]>frag_depth) continue;
//            bool discard = shader.fragment(c, color);
//            if (!discard) {
//                zbuffer[(int)(P.x+P.y*image.get_width())] = frag_depth;
//                image.set(P.x, P.y, color);
//            }
//        }
//    }
//}

float max_elevation_angle(float *zbuffer, Vec2f p, Vec2f dir) {
    float maxangle = 0;
    for (float t=0.; t<1000.; t+=1.) {
        Vec2f cur = p + dir*t;
        if (cur.x>=width || cur.y>=height || cur.x<0 || cur.y<0) return maxangle;

        float distance = (p-cur).norm();
        if (distance < 1.f) continue;
        float elevation = zbuffer[int(cur.x)+int(cur.y)*width]-zbuffer[int(p.x)+int(p.y)*width];
        maxangle = std::max(maxangle, atanf(elevation/distance));
    }
    return maxangle;
}

int main(int argc, char **argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
//        model = new Model("../obj/diablo3_pose.obj");
    }

    zbuffer = new float[width * height];

    for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage frame(width, height, TGAImage::RGB);
    lookat(eye, center, up);
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).norm());

    DepthShader depthshader;
    Vec4f screen_coords[3];
    for (int i = 0; i < model->nfaces(); i++) {
        for (int j = 0; j < 3; j++) {
            screen_coords[j] = depthshader.vertex(i, j);
        }
        triangle_shadow(screen_coords, depthshader, frame, zbuffer);
    }

    int count = width * height;
    int iter = 0;
    for (int x=0; x<width; x++) {
        for (int y=0; y<height; y++) {
            iter++;
//            std::cerr << "doing... remain : " << (count - iter) <<std::endl;
            if (zbuffer[x+y*width] < -1e5) {
//                std::cerr << "continue : " << (count - iter) << " " << zbuffer[x+y*width]  <<std::endl;
                continue;
            }
            float total = 0;
            for (float a=0; a<M_PI*2-1e-4; a += M_PI/4) {
                total += M_PI/2 - max_elevation_angle(zbuffer, Vec2f(x, y), Vec2f(cos(a), sin(a)));
            }
            total /= (M_PI/2)*8;
            total = pow(total, 10.f);

            std::cerr << "total : " << total  <<std::endl;

            frame.set(x, y, TGAColor(total*255, total*255, total*255));
        }
    }

    frame.flip_vertically();
    frame.write_tga_file("l8_ssao.tga");

    delete[] zbuffer;
    delete model;

    std::cerr << "finish" << std::endl;
    return 0;
}


