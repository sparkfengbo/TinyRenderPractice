//
// Created by fengbo on 2020/6/18.
//

#ifndef TINYRENDERPRACTICE_OUR_GL_H
#define TINYRENDERPRACTICE_OUR_GL_H


#include "geometry.h"
#include "tgaimage.h"

extern Matrix ModelView;
extern Matrix Viewport;
extern Matrix Projection;
const float depth = 2000.f;

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f);
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader {
    virtual ~IShader()= default;
    virtual Vec3i vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec3i *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);
void triangle_shadow(Vec4f *pts, IShader &shader, TGAImage &image, float *zbuffer);
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
Vec2f getV2fromV4(Vec4f v);
#endif //TINYRENDERPRACTICE_OUR_GL_H
