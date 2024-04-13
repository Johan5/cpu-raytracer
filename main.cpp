#include <iostream>

#include "game_object_container.h"
#include "sphere.h"
#include "camera.h"





int main() {
    std::cout << "Hello, World!" << std::endl;

    CameraConfig cameraConfig;
    Camera camera {Vec3d{0,0,0}, cameraConfig};
    GameObjectContainer world;
    world.add(std::make_unique<Sphere>(Vec3d{0.0, 0.0, -1.0}, 0.5));
    world.add(std::make_unique<Sphere>(Vec3d{0.0, -100.5, -1.0}, 100));
    camera.render(world);

    return 0;
}
