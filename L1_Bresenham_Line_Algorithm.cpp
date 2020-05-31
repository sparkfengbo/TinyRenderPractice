//
// Created by fengbo on 2020/5/31.
//

#include <iostream>
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

/**
 * 我的第一次尝试
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param image
 * @param color
 */
void lineMyCode(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    float k = (y1 - y0) * 1.f / (x1 - x0);
    for (int x = x0; x < x1; x++) {
        int y = k * (x - x0) + y0;
        image.set(x, y, color);
    }
}

/**
 * Lession 1 的第一种解法
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param image
 * @param color
 */
void line0(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (float t = 0; t < 1.f; t += .1f) {
        int x = x0 + t * (x1 - x0);
        int y = y0 + t * (y1 - y0);
        image.set(x, y, color);
    }
}

void line1(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float) (x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        image.set(x, y, color);
    }
}


/**
 * 比较接近Bresenham画线算法的一种方式
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param image
 * @param color `
 */
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {

    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float) (x1 - x0);
        int y = y0 * (1. - t) + y1 * t;

        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
    }
}


/**
 * 针对浮点运算的改进
 * @param x0
 * @param y0
 * @param x1
 * @param y1
 * @param image
 * @param color `
 */
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    float derror = dy / (float) dx;
    if (derror < 0) {
        derror = -derror;
    }
    float error = 0;

    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }

        error += derror;
        if (error > .5f) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1.f;
        }
    }
}


void bresenham_line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;

    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;

    int derror = std::abs(dy) * 2;
    int error = 0;
    int deta = dx * 2;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }

        error += derror;
        if (error > dx) {
            y += (y1 > y0 ? 1 : -1);
            error -= deta;
        }
    }
}

int main(int argc, char **argv) {
    std::cout << "Hello, World!" << std::endl;

    /**
     * my code
     */
    TGAImage image_my_code(100, 200, TGAImage::RGB);

    lineMyCode(13, 20, 80, 40, image_my_code, white);
    lineMyCode(20, 13, 40, 80, image_my_code, red);
    lineMyCode(80, 40, 13, 20, image_my_code, red);

    image_my_code.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_my_code.write_tga_file("image_my_code.tga");


    /**
     * line0
     */
    TGAImage image_line0(100, 200, TGAImage::RGB);

    line0(13, 20, 80, 40, image_line0, white);

    image_line0.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_line0.write_tga_file("image_line0.tga");

    /**
     * line1
     */
    TGAImage image_line1(100, 200, TGAImage::RGB);

    line1(13, 20, 80, 40, image_line1, white);
    //right to left   no line be drawn
    line1(40, 80, 20, 13, image_line1, white);
    line1(20, 13, 40, 80, image_line1, red);
    //right to left no line be drawn
    line1(80, 40, 13, 20, image_line1, red);
    line1(40, 40, 80, 40, image_line1, red);


    image_line1.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_line1.write_tga_file("image_line1.tga");


    /**
    * line2
    */
    TGAImage image_line2(100, 200, TGAImage::RGB);

    line2(13, 20, 80, 40, image_line2, white);
    line2(20, 13, 40, 80, image_line2, red);
    //right to left no line be drawn
    line2(40, 80, 20, 13, image_line2, white);
    line2(80, 40, 13, 20, image_line2, red);
    line2(40, 40, 80, 40, image_line2, red);


    image_line2.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_line2.write_tga_file("image_line2.tga");


    /**
    * line3
    */
    TGAImage image_line3(100, 200, TGAImage::RGB);

    line3(13, 20, 80, 40, image_line3, white);
    line3(20, 13, 40, 80, image_line3, red);
    //right to left
    line3(40, 80, 20, 13, image_line3, white);
    line3(80, 40, 13, 20, image_line3, red);
    line3(40, 40, 80, 40, image_line3, red);


    image_line3.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_line3.write_tga_file("image_line3.tga");


    /**
    * bresenham_line
    */
    TGAImage image_bresenham_line(100, 200, TGAImage::RGB);

//    bresenham_line(13, 20, 80, 40, image_bresenham_line, white);
//    bresenham_line(20, 13, 40, 80, image_bresenham_line, red);
    //right to left
    bresenham_line(40, 80, 20, 13, image_bresenham_line, white);
    bresenham_line(80, 40, 13, 20, image_bresenham_line, red);
    bresenham_line(40, 40, 80, 40, image_bresenham_line, red);

    image_bresenham_line.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image_bresenham_line.write_tga_file("image_bresenham_line.tga");

    return 0;
}

