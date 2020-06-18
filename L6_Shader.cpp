#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

const int width  = 800;
const int height = 800;
const int depth  = 255;

Model *model = NULL;
TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
Vec3f light_dir = Vec3f(1,-1,1).normalize();
Vec3f eye(1,1,3);
Vec3f center(0,0,0);


Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
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

struct Shader : public IShader {
    Vec2i varying_uv[3];
    float varying_inty[3];

    virtual Vec3i vertex(int iface, int nthvert) {
        varying_inty[nthvert] = model->norm(iface, nthvert)*light_dir;
        varying_uv[nthvert]   = model->uv(iface, nthvert);
        Vec3f gl_Vertex = model->vert(iface, nthvert);
        Vec3i gl_Position = vf2i(m2v(Viewport*Projection*ModelView*Matrix(gl_Vertex)));
        return gl_Position;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2i uv   = varying_uv[0]*bar.x + varying_uv[1]*bar.y + varying_uv[2]*bar.z;
//        float inty = varying_inty[0]*bar.x + varying_inty[1]*bar.y + varying_inty[2]*bar.z;
        //       inty = std::max(0.f, std::min(1.f, inty));
//        color = model->diffuse(uv)*inty;
        float inty = model->norm(uv)*light_dir;
        color = model->diffuse(uv)*inty;
        return false;
    }
};

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    lookat(eye, center, Vec3f(0,1,0));
    viewport(width/8, height/8, width*3/4, height*3/4);
    projection(-1.f/(eye-center).norm());

    TGAImage image(width, height, TGAImage::RGB);
    Shader shader;
    for (int i=0; i<model->nfaces(); i++) {
        Vec3i screen_coords[3];
        for (int j=0; j<3; j++) {
            screen_coords[j] = shader.vertex(i, j);
        }
        triangle(screen_coords, shader, image, zbuffer);
    }
    image.flip_vertically();
    image.write_tga_file("L6_output.tga");

    zbuffer.flip_vertically();
    zbuffer.write_tga_file("L6_zbuffer.tga");
    delete model;
    return 0;
}

