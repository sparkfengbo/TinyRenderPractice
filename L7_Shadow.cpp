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

struct DepthShader{
    Vec4f m2v(Matrix m) {
        return Vec4f(m[0][0], m[1][0], m[2][0],m[3][0]);
    }

    Vec3f projectDivision(Vec4f v) {
        return Vec3f(v[0]/v[3], v[1]/v[3],v[2]/v[3]);
    }

    Vec3f varying_tri[3];

    Vec4f vertex4f(int iface, int nthvert) {
        Vec3f vertex = model->vert(iface, nthvert);
        Vec4f gl_vertex(vertex.x, vertex.y, vertex.z, 1);

        if (iface == 0) {
            std::cerr<<"0 - gl_vertex = "<<gl_vertex<<std::endl;
        }

        gl_vertex = m2v(Viewport * Projection * ModelView * gl_vertex);
        if (iface == 0) {
            std::cerr<<"1 - gl_vertex = "<<gl_vertex<<std::endl;
        }

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

int xx = 0;

void triangle_shadow(Vec4f *pts, DepthShader &shader, TGAImage &image, float *zbuffer) {
    Vec2f bboxmin( std::numeric_limits<int>::max(),  std::numeric_limits<int>::max());
    Vec2f bboxmax(-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max());
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::min(bboxmin[j], pts[i][j]);
            bboxmax[j] = std::max(bboxmax[j], pts[i][j]);
        }
    }
    Vec2i P;
    TGAColor color;

    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec2f a = getV2fromV4(pts[0]/pts[0][3]);
            Vec2f b = getV2fromV4(pts[1]/pts[1][3]);
            Vec2f e = (getV2fromV4(pts[2]/pts[2][3]));
            if (pts[0][0] -  253.585 < 1e-2 && xx < 4) {
                std::cerr<<"barycentric#0 = " <<  a <<std::endl;
                std::cerr<<"barycentric#1 = " <<  b<<std::endl;
                std::cerr<<"barycentric#2 = " <<  e <<std::endl;
                std::cerr<<"barycentric#3 = " <<  P <<std::endl;
            }
            Vec3f c = barycentric(getV2fromV4(pts[0]/pts[0][3]), getV2fromV4(pts[1]/pts[1][3]), getV2fromV4(pts[2]/pts[2][3]), Vec2f(P.x, P.y));

            if (pts[0][0] -  253.585 < 1e-2 && xx < 4) {
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

        std::cerr<<"ModelView = " << ModelView<<std::endl;
        std::cerr<<"Projection = " <<Projection<<std::endl;
        std::cerr<<"Viewport = " <<Viewport<<std::endl;

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
        depth.write_tga_file("depth.tga");
    }

    Matrix M = Viewport*Projection*ModelView;

//    { // rendering the frame buffer
//        TGAImage frame(width, height, TGAImage::RGB);
//        lookat(eye, center, up);
//        viewport(width/8, height/8, width*3/4, height*3/4);
//        projection(-1.f/(eye-center).norm());
//
//        Shader shader(ModelView, (Projection*ModelView).invert_transpose(), M*(Viewport*Projection*ModelView).invert());
//        Vec4f screen_coords[3];
//        for (int i=0; i<model->nfaces(); i++) {
//            for (int j=0; j<3; j++) {
//                screen_coords[j] = shader.vertex(i, j);
//            }
//            triangle(screen_coords, shader, frame, zbuffer);
//        }
//        frame.flip_vertically(); // to place the origin in the bottom left corner of the image
//        frame.write_tga_file("framebuffer.tga");
//    }

    delete model;
    delete [] zbuffer;
    delete [] shadowbuffer;
    return 0;
}


