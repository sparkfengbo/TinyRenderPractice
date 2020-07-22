cmake_minimum_required(VERSION 3.16)
project(TinyRenderPractice)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lession1 tgaimage.cpp tgaimage.h L1_Bresenham_Line_Algorithm.cpp)

add_executable(Lession1_load_model tgaimage.cpp tgaimage.h L1_load_model_obj.cpp model.cpp model.h geometry.h)

add_executable(Lession2 tgaimage.cpp tgaimage.h L2_Triangle.cpp model.cpp model.h geometry.h)

add_executable(Lession3 tgaimage.cpp tgaimage.h L3_zBuffer.cpp model.cpp model.h geometry.h)

add_executable(Lession4 tgaimage.cpp tgaimage.h L4_PerspectiveProjection.cpp model.cpp model.h geometry.h geometry.cpp)

add_executable(Lession5 tgaimage.cpp tgaimage.h L5_Camera.cpp model.cpp model.h geometry.h geometry.cpp)

add_executable(Lession6 tgaimage.cpp tgaimage.h L6_Shader.cpp model.cpp model.h geometry.h geometry.cpp our_gl.cpp our_gl.h)

add_executable(Lession7 tgaimage.cpp tgaimage.h L7_Shadow.cpp model.cpp model.h geometry.h geometry.cpp our_gl.cpp our_gl.h)

add_executable(Lession8 tgaimage.cpp tgaimage.h L8_ambient_occlusion.cpp model.cpp model.h geometry.h geometry.cpp our_gl.cpp our_gl.h)

add_executable(Lession8_ssao tgaimage.cpp tgaimage.h L8_ssao.cpp model.cpp model.h geometry.h geometry.cpp our_gl.cpp our_gl.h)




