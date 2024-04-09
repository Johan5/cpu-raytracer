cmake_minimum_required(VERSION 3.27)
project(raytracer)

set(CMAKE_CXX_STANDARD 20)

add_executable(raytracer main.cpp
        image.h
        rgb.h
        ray.h
        sphere.h)

target_include_directories(raytracer PRIVATE thirdparty thirdparty/eigen)
