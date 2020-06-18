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

void viewport(int x, int y, int w, int h);
void projection(float coeff=0.f); // coeff = -1/c
void lookat(Vec3f eye, Vec3f center, Vec3f up);

struct IShader {
    virtual ~IShader()= default;
    virtual Vec3i vertex(int iface, int nthvert) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
};

void triangle(Vec3i *pts, IShader &shader, TGAImage &image, TGAImage &zbuffer);

#endif //TINYRENDERPRACTICE_OUR_GL_H
