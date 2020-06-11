//
// Created by fengbo on 2020/5/31.
//

#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
#include <vector>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0,   255, 0,   255);
const TGAColor blue  = TGAColor(0,   0,   255, 255);
const int width  = 800;
const int height = 500;


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


void rasterize(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color, int ybuffer[]) {

    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.-t) + p1.y*t;
        if (ybuffer[x] < y) {
            ybuffer[x] = y;
            image.set(x, 0, color);
        }
    }
}

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u =  s[0] ^ s[1];
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f *pts, TGAImage &image, TGAColor color, float* zbuffer) {
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
            if (zbuffer[int(P.x+P.y*width)]<P.z) {
                zbuffer[int(P.x+P.y*width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}
int main(int argc, char **argv) {
    /**
     * y-buffer
     */
    { // just dumping the 2d scene (yay we have enough dimensions!)
        TGAImage scene(width, height, TGAImage::RGB);

        // scene "2d mesh"
        line(Vec2i(20, 34),   Vec2i(744, 400), scene, red);
        line(Vec2i(120, 434), Vec2i(444, 400), scene, green);
        line(Vec2i(330, 463), Vec2i(594, 200), scene, blue);

        // screen line
        line(Vec2i(10, 10), Vec2i(790, 10), scene, white);

        scene.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        scene.write_tga_file("scene.tga");
    }

    {
        TGAImage render(width, 16, TGAImage::RGB);

        int ybuffer[width];
        for (int i=0; i<width; i++) {
            ybuffer[i] = std::numeric_limits<int>::min();
        }

        rasterize(Vec2i(20, 34),   Vec2i(744, 400), render, red, ybuffer);
        rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green,ybuffer);
        rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue, ybuffer);

        // 1-pixel wide image is bad for eyes, lets widen it
        for (int i=0; i<width; i++) {
            for (int j=1; j<16; j++) {
                render.set(i, j, render.get(i, 0));
            }
        }

        render.flip_vertically();
        render.write_tga_file("render.tga");
    }


    Model *model = new Model("../obj/african_head.obj");
    int image_african_head_size = 800;

    float *zbuffer = new float[image_african_head_size*image_african_head_size];
    for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

    TGAImage image_african_z_buffer(image_african_head_size, image_african_head_size, TGAImage::RGB);
    Vec3f light_dir(0,0,-1);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3f((v.x + 1.) * image_african_head_size / 2., (v.y + 1.) * image_african_head_size / 2., v.z);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^(world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0) {
            Vec3f pts[3];
            for (int i=0; i<3; i++) pts[i] = screen_coords[i];
            triangle(pts, image_african_z_buffer,
                      TGAColor(intensity * 255, intensity * 255, intensity * 255, 255), zbuffer);
        }
//        Vec3f pts[3];
//        for (int i=0; i<3; i++) pts[i] = screen_coords[i];
//        triangle(pts, image_african_z_buffer,
//                 TGAColor(rand()%255, rand()%255, rand()%255, 255), zbuffer);

    }
    image_african_z_buffer.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_african_z_buffer.write_tga_file("image_african_z_buffer.tga");

    delete model;

    return 0;
}