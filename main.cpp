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
    Material leftShellMaterial;
    leftShellMaterial._reflectanceType = ReflectanceType::Dielectric;
    // A shell of something thicker, e.g. glass
    leftShellMaterial._refractionIndex = 1.5;
    Material leftCoreMaterial;
    leftCoreMaterial._reflectanceType = ReflectanceType::Dielectric;
    // A core of air
    leftCoreMaterial._refractionIndex = 1.0 / leftShellMaterial._refractionIndex;
    Material rightMaterial;
    rightMaterial._reflectanceType = ReflectanceType::Metal;
    rightMaterial._albedo = {0.8, 0.6, 0.2};
    rightMaterial._metalFuzz = 1.0;

    GameObjectContainer world;
    world.add(std::make_unique<Sphere>(Vec3d{0.0, -100.5, -1.0}, 100.0, groundMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{0.0, 0.0, -1.2}, 0.5, centerMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{-1.0, 0.0, -1.0}, 0.5, leftShellMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{-1.0, 0.0, -1.0}, 0.4, leftCoreMaterial));
    world.add(std::make_unique<Sphere>(Vec3d{1.0, 0.0, -1.0}, 0.5, rightMaterial));
    camera.render(world);

    return 0;
}
