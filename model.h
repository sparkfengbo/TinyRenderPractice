//
// Created by fengbo on 2020/5/31.
//

#ifndef TINYRENDERPRACTICE_MODEL_H
#define TINYRENDERPRACTICE_MODEL_H

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<Vec3i> > faces_; // attention, this Vec3i means vertex/uv/normal
    std::vector<Vec3f> norms_;
    std::vector<Vec2f> uv_;
    TGAImage diffusemap_;
    TGAImage normalmap_;
    void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f norm(int iface, int nvert);
    Vec3f norm(Vec2i uv);
    Vec3f vert(int i);
    Vec3f vert(int iface, int nthvert);
    Vec2i uv(int iface, int nvert);
    TGAColor diffuse(Vec2i uv);
    std::vector<int> face(int idx);

};
#endif //TINYRENDERPRACTICE_MODEL_H
