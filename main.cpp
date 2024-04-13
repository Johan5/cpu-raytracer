#include <iostream>

#include "game_object_container.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


int main() {
    std::cout << "Hello, World!" << std::endl;

    CameraConfig cameraConfig;
    Camera camera{Vec3d{0, 0, 0}, cameraConfig};
    Material groundMaterial;
    groundMaterial._albedo = {0.8, 0.8, 0.0};
    Material centerMaterial;
    centerMaterial._albedo = {0.1, 0.2, 0.5};
    Material leftMaterial;
    leftMaterial._reflectanceType = ReflectanceType::Metal;
    leftMaterial._albedo = {0.8, 0.8, 0.8};
    leftMaterial._metalFuzz = 0.3;
    Material rightMaterial;
    rightMaterial._reflectanceType = ReflectanceType::Metal;
    rightMaterial._albedo = {0.8, 0.6, 0.2};
    rightMaterial._metalFuzz = 1.0;

    GameObjectContainer world;
    world.add(std::make_unique<Sphere>(Vec3d{0.0, -100.5, -1.0}, 100.0, groundMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{0.0, 0.0, -1.2}, 0.5, centerMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{-1.0, 0.0, -1.0}, 0.5, leftMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{1.0, 0.0, -1.0}, 0.5, rightMaterial));
    camera.render(world);

    return 0;
}
