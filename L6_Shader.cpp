#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

const int width = 800;
const int height = 800;
const int depth = 255;

Model *model = NULL;
Vec3f center(0, 0, 0);
Vec3f light_dir(1,1,0);
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

struct GouraudShader : public IShader {
    Vec2i varying_uv[3];
    Vec3f varying_inty;
    virtual Vec3i vertex(int iface, int nthvert) {
        varying_inty[nthvert] = std::max(0.f, model->norm(iface, nthvert) * light_dir);
        varying_uv[nthvert] = model->uv(iface, nthvert);
        Vec3f gl_Vertex = model->vert(iface, nthvert);
        Vec3i gl_Position = vf2i(m2v(Viewport * Projection * ModelView * Matrix(gl_Vertex)));
        return gl_Position;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        /**
         * 实际上是利用顶点法线计算光强度，然后插值，乘以纹理计算后的颜色
         */
        float inty = varying_inty[0] * bar.x + varying_inty[1] * bar.y + varying_inty[2] * bar.z;
        inty = std::max(0.f, std::min(1.f, inty));
        color = model->diffuse(uv) * inty;
        return false;
    }

//        float intensity = varying_inty * bar;
//        if (intensity>.85) intensity = 1;
//        else if (intensity>.60) intensity = .80;
//        else if (intensity>.45) intensity = .60;
//        else if (intensity>.30) intensity = .45;
//        else if (intensity>.15) intensity = .30;
//        else intensity = 0;
//        color = TGAColor(255, 155, 0)*intensity;
//        color = model->diffuse(uv)*intensity;
};


struct PhongShader : public IShader {
    Vec2i varying_uv[3];
    Vec3f varying_norm[3];
    virtual Vec3i vertex(int iface, int nthvert) {
        varying_norm[nthvert] = model->norm(iface, nthvert);
        varying_uv[nthvert] = model->uv(iface, nthvert);
        Vec3f gl_Vertex = model->vert(iface, nthvert);
        Vec3i gl_Position = vf2i(m2v(Viewport * Projection * ModelView * Matrix(gl_Vertex)));
        return gl_Position;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        Vec3f bn = Vec3f(
                varying_norm[0].x * bar.x + varying_norm[1].x * bar.y +varying_norm[2].x * bar.z,
                varying_norm[0].y * bar.x + varying_norm[1].y * bar.y +varying_norm[2].y * bar.z,
                varying_norm[0].z * bar.x + varying_norm[1].z * bar.y +varying_norm[2].z * bar.z
                );
        bn.normalize();
        float inty =std::max(0.f, bn * light_dir);
        color = model->diffuse(uv) * inty;
        return false;
    }
};



/**
 * 法线贴图
 */
struct NormalShader : public IShader {
    Vec2i varying_uv[3];

    virtual Vec3i vertex(int iface, int nthvert) {
        varying_uv[nthvert] = model->uv(iface, nthvert);
        Vec3f gl_Vertex = model->vert(iface, nthvert);
        Vec3i gl_Position = vf2i(m2v(Viewport * Projection * ModelView * Matrix(gl_Vertex)));
        return gl_Position;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        float inty = model->norm(uv) * light_dir;
        color = model->diffuse(uv) * inty;
        return false;
    }
};

/**
 * 法线贴图 + 镜面反射
 */
struct SpecularShader : public IShader {
    Vec2i varying_uv[3];
    Vec3f varying_inty;

    virtual Vec3i vertex(int iface, int nthvert) {
        varying_inty[nthvert] = model->norm(iface, nthvert) * light_dir;
        varying_uv[nthvert] = model->uv(iface, nthvert);
        Vec3f gl_Vertex = model->vert(iface, nthvert);
        Vec3i gl_Position = vf2i(m2v(Viewport * Projection * ModelView * Matrix(gl_Vertex)));
        return gl_Position;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        Vec2i uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;
        Matrix matrix = Projection * ModelView;
        //法向量不能随便使用矩阵转换空间，尤其是发生scale的情况下，要使用矩阵的逆矩阵转置矩阵
        Vec3f n = m2v((matrix.inverse().transpose() * model->norm(uv))).normalize();
        //转到世界空间
        Vec3f l = m2v((ModelView * light_dir)).normalize();
        Vec3f r = (n*(n*l*2.f) - l).normalize();   // reflected light
        float spec = pow(std::max(r.z, 0.0f), 4);
//        float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
        float diff = std::max(0.f, n*l);
        TGAColor c = model->diffuse(uv);
        color = c;
        for (int i=0; i<3; i++) {
            //5 for the ambient component, 1 for the diffuse component and .6 for the specular component.
            color[i] = std::min<float>(20 + c[i]*(1.2*diff + .9*spec), 255);
        }
        return false;
    }
};

int main(int argc, char **argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    lookat(eye, center, up);
    viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
    projection(-1.f / (eye - center).norm());

    {
        TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
        TGAImage image(width, height, TGAImage::RGB);
        GouraudShader shader;
        for (int i = 0; i < model->nfaces(); i++) {
            Vec3i screen_coords[3];
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, image, zbuffer);
        }
        image.flip_vertically();
        image.write_tga_file("L6_output_gouraud.tga");

        zbuffer.flip_vertically();
        zbuffer.write_tga_file("L6_zbuffer_gouraud.tga");
    }

    {
        TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
        TGAImage image(width, height, TGAImage::RGB);
        PhongShader shader;
        for (int i = 0; i < model->nfaces(); i++) {
            Vec3i screen_coords[3];
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, image, zbuffer);
        }
        image.flip_vertically();
        image.write_tga_file("L6_output_phong.tga");

        zbuffer.flip_vertically();
        zbuffer.write_tga_file("L6_zbuffer_phong.tga");
    }

    {
        TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
        TGAImage image(width, height, TGAImage::RGB);
        NormalShader shader;
        for (int i = 0; i < model->nfaces(); i++) {
            Vec3i screen_coords[3];
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, image, zbuffer);
        }
        image.flip_vertically();
        image.write_tga_file("L6_output_normal_mapping.tga");

        zbuffer.flip_vertically();
        zbuffer.write_tga_file("L6_zbuffer_normal_mapping.tga");
    }

    {
        TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
        TGAImage image(width, height, TGAImage::RGB);
        SpecularShader shader;
        for (int i = 0; i < model->nfaces(); i++) {
            Vec3i screen_coords[3];
            for (int j = 0; j < 3; j++) {
                screen_coords[j] = shader.vertex(i, j);
            }
            triangle(screen_coords, shader, image, zbuffer);
        }
        image.flip_vertically();
        image.write_tga_file("L6_output_specular.tga");

        zbuffer.flip_vertically();
        zbuffer.write_tga_file("L6_zbuffer_specular.tga");
    }

    delete model;
    return 0;
}

