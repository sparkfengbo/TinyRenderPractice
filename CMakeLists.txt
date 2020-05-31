cmake_minimum_required(VERSION 3.16)
project(TinyRenderPractice)

set(CMAKE_CXX_STANDARD 14)

add_executable(Lession1 tgaimage.cpp tgaimage.h L1_Bresenham_Line_Algorithm.cpp)

add_executable(Lession1_load_model tgaimage.cpp tgaimage.h L1_load_model_obj.cpp model.cpp model.h geometry.h)
