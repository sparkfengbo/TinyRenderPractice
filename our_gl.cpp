//
// Created by fengbo on 2020/6/18.
//

#include "our_gl.h"
#include <cmath>
#include <vector>
#include <limits>
#include "tgaimage.h"
#include "geometry.h"


Matrix ModelView;
Matrix Viewport;
Matrix Projection;

void viewport(int x, int y, int w, int h) {
    Viewport = Matrix::identity(4);
    Viewport[0][3] = x+w/2.f;
    Viewport[1][3] = y+h/2.f;
    Viewport[2][3] = 255.f/2.f;
    Viewport[0][0] = w/2.f;
    Viewport[1][1] = h/2.f;
    Viewport[2][2] = 255.f/2.f;
}

void projection(float coeff) {
    Projection = Matrix::identity(4);
    Projection[3][2] = coeff;
}

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (up^z).normalize();
    Vec3f y = (z^x).normalize();
    ModelView = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        ModelView[0][i] = x[i];
        ModelView[1][i] = y[i];
        ModelView[2][i] = z[i];
        ModelView[i][3] = -center[i];
    }
}

Vec3f barycentric(Vec3i A, Vec3i B, Vec3i C, Vec3i P) {
    Vec3f u = Vec3f(C.x-A.x, B.x-A.x, A.x-P.x)^Vec3f(C.y-A.y, B.y-A.y, A.y-P.y);
    return std::abs(u.z)>.5 ? Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z) : Vec3f(-1,1,1); // dont forget that u.z is an integer. If it is zero then triangle ABC is degenerate
}

void triangle(Vec3i *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer) {
    Vec2i bboxmin( std::numeric_limits<int>::max(),  std::numeric_limits<int>::max());
    Vec2i bboxmax(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]);
        }
    }
    Vec3i P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f c = barycentric(pts[0], pts[1], pts[2], P);
            P.z = std::max(0, std::min(255, int(pts[0].z*c.x + pts[1].z*c.y + pts[2].z*c.z + .5))); // clamping to 0-255 since it is stored in unsigned char
            if (c.x<0 || c.y<0 || c.z<0 || zbuffer.get(P.x, P.y)[0]>P.z) continue;
            TGAColor color;
            bool discard = shader.fragment(c, color);
            if (!discard) {
                zbuffer.set(P.x, P.y, TGAColor(P.z));
                image.set(P.x, P.y, color);
            }
        }
    }
//    image.set(j, (t0.y+i), TGAColor(color.bgra[2]*ityP, color.bgra[1]*ityP, color.bgra[0]*ityP));
}