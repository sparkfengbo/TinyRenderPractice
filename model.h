//
// Created by fengbo on 2020/5/31.
//

#ifndef TINYRENDERPRACTICE_MODEL_H
#define TINYRENDERPRACTICE_MODEL_H

#include <vector>
#include "geometry.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> > faces_;
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
};
#endif //TINYRENDERPRACTICE_MODEL_H
