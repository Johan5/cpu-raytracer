cmake_minimum_required(VERSION 3.27)
project(raytracer)

set(CMAKE_CXX_STANDARD 20)

add_executable(raytracer main.cpp
        image.h
        ray.h
        sphere.h
        game_object.h
        game_object_container.h
        vec.h
        math_utils.h
        interval.h
        camera.h
        material.h)

target_include_directories(raytracer PRIVATE thirdparty thirdparty/eigen)
