cmake_minimum_required(VERSION 3.16)
project(TinyRenderPractice)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lession1 tgaimage.cpp tgaimage.h L1_Bresenham_Line_Algorithm.cpp L2_Triangle.cpp)

add_executable(Lession1_load_model tgaimage.cpp tgaimage.h L1_load_model_obj.cpp model.cpp model.h geometry.h)

add_executable(Lession2 tgaimage.cpp tgaimage.h L2_Triangle.cpp model.cpp model.h geometry.h)

add_executable(Lession3 tgaimage.cpp tgaimage.h L3_zBuffer.cpp model.cpp model.h geometry.h)

add_executable(Lession4 tgaimage.cpp tgaimage.h L4_PerspectiveProjection.cpp model.cpp model.h geometry.h geometry.cpp)


