//
// Created by fengbo on 2020/5/31.
//

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <vector>


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const int width = 200;
const int height = 200;


void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color) {
    bool steep = false;

    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        steep = true;
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
    }

    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    int dy = p1.y - p0.y;
    int dx = p1.x - p0.x;

    int derror = 2 * std::abs(dy);
    int error = 0;
    int delta = 2 * dx;
    int y = p0.y;
    for (int x = p0.x; x <= p1.x; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (p1.y > p0.y ? 1 : -1);
            error -= delta;
        }
    }
}


/**
 * 三角形 第一次尝试, stroke模式画三角形
 * @param t0
 * @param t1
 * @param t2
 * @param image
 * @param color
 */
void triangle0(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    line(t0, t1, image, color);
    line(t1, t2, image, color);
    line(t2, t0, image, color);
}

void triangle1(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y > t1.y) {
        std::swap(t0, t1);
    }

    if (t0.y > t2.y) {
        std::swap(t0, t2);
    }

    if (t1.y > t2.y) {
        std::swap(t1, t2);
    }

    line(t0, t1, image, green);
    line(t1, t2, image, green);
    line(t2, t0, image, red);
}

void triangle2(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y > t1.y) {
        std::swap(t0, t1);
    }

    if (t0.y > t2.y) {
        std::swap(t0, t2);
    }

    if (t1.y > t2.y) {
        std::swap(t1, t2);
    }

    int total_height = t2.y - t0.y + 1;
    int segment_height = t1.y - t0.y + 1;
    for (int y = t0.y; y <= t1.y; y++) {
        float alpha = (float) (y - t0.y) / total_height;
        float beta = (float) (y - t0.y) / segment_height;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;
        image.set(A.x, y, red);
        image.set(B.x, y, green);
    }
}

void triangle3(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y > t1.y) {
        std::swap(t0, t1);
    }

    if (t0.y > t2.y) {
        std::swap(t0, t2);
    }

    if (t1.y > t2.y) {
        std::swap(t1, t2);
    }

    int total_height = t2.y - t0.y + 1;
    int segment_height_down = t1.y - t0.y + 1;
    int segment_height_up = t2.y - t1.y + 1;

    //上半部分填充
    for (int y = t1.y; y <= t2.y; y++) {
        float alpha = float (y - t0.y) / total_height;
        float beta = float (y - t1.y) / segment_height_up;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t1 + (t2 - t1) * beta;

        if (A.x > B.x) {
            std::swap(A,B);
        }
        for(int x = A.x; x <= B.x; x++) {
            image.set(x, y, color);
        }
    }

    //下半部分填充
    for (int y = t0.y; y <= t1.y; y++) {
        float alpha = (float) (y - t0.y) / total_height;
        float beta  = (float) (y - t0.y) / segment_height_down;
        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = t0 + (t1 - t0) * beta;

        if (A.x > B.x) {
            std::swap(A,B);
        }
        for(int x = A.x; x <= B.x; x++) {
            image.set(x, y, color);
        }
    }
}

/**
 * 合并上下部分填充循环
 * @param t0
 * @param t1
 * @param t2
 * @param image
 * @param color
 */
void triangle4(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {
    if (t0.y > t1.y)  std::swap(t0, t1);
    if (t0.y > t2.y)  std::swap(t0, t2);
    if (t1.y > t2.y)  std::swap(t1, t2);

    int total_height = t2.y - t0.y + 1;
    int segment_height_down = t1.y - t0.y + 1;
    int segment_height_up = t2.y - t1.y + 1;

    for (int i = 0; i < total_height; i++) {
        bool up = i > segment_height_down - 1;
        float alpha = (float) i / total_height;
        float beta = (float) (i - (up ? segment_height_down - 1 : 0)) / (up ? segment_height_up : segment_height_down); // be careful: with above conditions no division by zero here

        Vec2i A =      t0 + (t2 - t0) * alpha;
        Vec2i B = up ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

        if (A.x > B.x) std::swap(A, B);

        for (int j = A.x; j <= B.x; j++) {
            image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
        }
    }
}

//TODO
//Vec3f barycentric(Vec2i *pts, Vec2i P) {
//    Vec3f u = cross(Vec3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]), Vec3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]));
//}

void barycentric_triangle(Vec2i *pts, TGAImage &image, TGAColor color) {
//    Vec2i rectFMax(0, 0);
//    Vec2i rectFMin(image.get_width() - 1, image.get_height() - 1);
//
//    Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
//    for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 2; j++) {
//            rectFMin[j] = std::max(0, std::min(rectFMin[j], pts[i][j]));
//            rectFMax[j] = std::min(clamp[j], std::max(rectFMax[j], pts[i][j]));
//        }
//    }
//
//
//    Vec2i P;
//    for (P.x = rectFMin.x; P.x <= rectFMax.x; P.x++) {
//        for (P.y = rectFMin.y; P.y <= rectFMax.y; P.y++) {
//            Vec3f bc_screen = barycentric(pts, P);
//            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
//            image.set(P.x, P.y, color);
//        }
//    }

//    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1);
//    Vec2i bboxmax(0, 0);
//    Vec2i clamp(image.get_width()-1, image.get_height()-1);
//    for (int i=0; i<3; i++) {
//        for (int j=0; j<2; j++) {
//            bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j]));
//            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
//        }
//    }
//    Vec2i P;
//    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
//        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
//            Vec3f bc_screen  = barycentric(pts, P);
//            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
//            image.set(P.x, P.y, color);
//        }
//    }
}

int main(int argc, char **argv) {
    TGAImage image_triangle0(width, height, TGAImage::RGB);

    Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
    Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};


    triangle0(t0[0], t0[1], t0[2], image_triangle0, red);
    triangle0(t1[0], t1[1], t1[2], image_triangle0, white);
    triangle0(t2[0], t2[1], t2[2], image_triangle0, green);

    image_triangle0.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_triangle0.write_tga_file("image_triangle0.tga");


    TGAImage image_triangle1(width, height, TGAImage::RGB);

    triangle1(t0[0], t0[1], t0[2], image_triangle1, red);
    triangle1(t1[0], t1[1], t1[2], image_triangle1, white);
    triangle1(t2[0], t2[1], t2[2], image_triangle1, green);

    image_triangle1.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_triangle1.write_tga_file("image_triangle1.tga");

    TGAImage image_triangle2(width, height, TGAImage::RGB);

    triangle2(t0[0], t0[1], t0[2], image_triangle2, red);
    triangle2(t1[0], t1[1], t1[2], image_triangle2, white);
    triangle2(t2[0], t2[1], t2[2], image_triangle2, green);

    image_triangle2.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_triangle2.write_tga_file("image_triangle2.tga");


    TGAImage image_triangle3(width, height, TGAImage::RGB);

    triangle3(t0[0], t0[1], t0[2], image_triangle3, red);
    triangle3(t1[0], t1[1], t1[2], image_triangle3, white);
    triangle3(t2[0], t2[1], t2[2], image_triangle3, green);

    image_triangle3.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_triangle3.write_tga_file("image_triangle3.tga");

    TGAImage image_triangle4(width, height, TGAImage::RGB);

    triangle4(t0[0], t0[1], t0[2], image_triangle4, red);
    triangle4(t1[0], t1[1], t1[2], image_triangle4, white);
    triangle4(t2[0], t2[1], t2[2], image_triangle4, green);

    image_triangle4.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_triangle4.write_tga_file("image_triangle4.tga");


//    TGAImage image_triangle_barycentric(width, height, TGAImage::RGB);
//
//    barycentric_triangle(t0, image_triangle_barycentric, red);
//    barycentric_triangle(t1, image_triangle_barycentric, white);
//    barycentric_triangle(t2, image_triangle_barycentric, green);
//
//    image_triangle_barycentric.flip_vertically(); // i want to have the origin at the left bottom corner of the image
//    image_triangle_barycentric.write_tga_file("image_triangle_barycentric.tga");


    Model *model = new Model("../obj/african_head.obj");
    int image_african_head_size = 800;
    TGAImage image_african_head(image_african_head_size, image_african_head_size, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f world_coords = model->vert(face[j]);
            screen_coords[j] = Vec2i((world_coords.x + 1.) * image_african_head_size / 2., (world_coords.y + 1.) * image_african_head_size / 2.);
        }
        triangle4(screen_coords[0], screen_coords[1], screen_coords[2], image_african_head,
                  TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
    }
    image_african_head.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_african_head.write_tga_file("triangle_african_head.tga");


    TGAImage image_african_head_light(image_african_head_size, image_african_head_size, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * image_african_head_size / 2., (v.y + 1.) * image_african_head_size / 2.);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^(world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            triangle4(screen_coords[0], screen_coords[1], screen_coords[2], image_african_head_light,
                      TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
    }
    image_african_head_light.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_african_head_light.write_tga_file("image_african_head_light.tga");

    delete model;
    return 0;
}