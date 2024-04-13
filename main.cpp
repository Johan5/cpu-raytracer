#include <iostream>

#include "game_object_container.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"


int main() {
    std::cout << "Hello, World!" << std::endl;

    CameraConfig cameraConfig;
    cameraConfig._imgWidth = 1200;
    cameraConfig._samplesPerPixel = 500;
    cameraConfig._lookFrom = {13, 2, 3};
    cameraConfig._lookAt = {0, 0, 0};
    cameraConfig._viewUp = {0, 1, 0};
    cameraConfig._verticalFieldOfView = 20;
    cameraConfig._focusDist = 10.0;
    cameraConfig._defocusAngle = 0.6;
    Camera camera{cameraConfig};

    GameObjectContainer world;

    Material groundMaterial;
    groundMaterial._albedo = {0.5, 0.5, 0.5};
    world.add(std::make_unique<Sphere>(Vec3d{0.0, -1000, 0.0}, 1000.0, groundMaterial));

    for (int32_t a = -11; a < 11; ++a) {
        for (int32_t b = -11; b < 11; ++b) {
            double chooseMat = randomDouble();
            Vec3d center{a + 0.9 * randomDouble(), 0.2, b + 0.9 * randomDouble()};

            if ((center - Vec3d{4, 0.2, 0}).norm() > 0.9) {
                Material material;
                if (chooseMat < 0.8) {
                    material._reflectanceType = ReflectanceType::Lambertian;
                    material._albedo = randomVec().cwiseProduct(randomVec());
                } else if (chooseMat < 0.95) {
                    material._reflectanceType = ReflectanceType::Metal;
                    material._albedo = randomVec(0.5, 1.0);
                    material._metalFuzz = randomDouble(0, 0.5);
                } else {
                    material._reflectanceType = ReflectanceType::Dielectric;
                    material._refractionIndex = 1.5;
                }
                world.add(std::make_unique<Sphere>(center, 0.2, material));
            }
        }
    }

    Material glass;
    glass._reflectanceType = ReflectanceType::Dielectric;
    glass._refractionIndex = 1.5;
    world.add(std::make_unique<Sphere>(Vec3d{0, 1, 0}, 1.0, glass));

    Material diffuse;
    diffuse._reflectanceType = ReflectanceType::Lambertian;
    diffuse._albedo = {0.4, 0.2, 0.1};
    world.add(std::make_unique<Sphere>(Vec3d{-4, 1, 0}, 1.0, diffuse));

    Material metal;
    metal._reflectanceType = ReflectanceType::Metal;
    metal._albedo = {0.7, 0.6, 0.5};
    metal._metalFuzz = 0.0;
    world.add(std::make_unique<Sphere>(Vec3d{4, 1, 0}, 1.0, metal));

    camera.render(world);

    return 0;
}
