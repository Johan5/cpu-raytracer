#include <iostream>

#include "image.h"
#include "sphere.h"
#include "ray.h"

#include "Eigen/Core"

void printNiceImg() {

    int32_t width = 256;
    int32_t height = 256;
    std::vector<Eigen::Vector3d> pixels;
    for (int32_t row = 0; row < height; ++row) {
        std::cout << "\rScanlines remaining: " << (height - row) << ' ' << std::flush;
        for (int32_t col = 0; col < width; ++col) {
            double r = static_cast<double>(col) / (width - 1);
            double g = static_cast<double>(row) / (height - 1);
            double b = 0.0;

            pixels.push_back(Eigen::Vector3d{r, g, b});
        }
    }
    std::cout << "\rDone.                 \n";

    Image<Eigen::Vector3d> image{std::move(pixels), width, height, 3};
    image.writeToFile(std::filesystem::path{"./"});
}

Eigen::Vector3d rayColor(const Ray &ray) {
    Sphere sphere{Eigen::Vector3d{0, 0, -1}, 0.5};
    std::optional<Eigen::Vector3d> impact = sphere.rayIntersection(ray);
    if (impact.has_value()) {
        Eigen::Vector3d impactNormal = (impact.value() - sphere.getCentre()).normalized();
        return Eigen::Vector3d{impactNormal.x()+1, impactNormal.y()+1, impactNormal.z()+1} / 2.0;
    }

    auto blue = Eigen::Vector3d{0.5, 0.7, 1.0};
    auto white = Eigen::Vector3d{1.0, 1.0, 1.0};
    auto dir = ray.getDirection().normalized();
    auto a = (dir.y() + 1.0) / 2.0;
    return (1.0 - a) * white + a * blue;
}

int main() {
    std::cout << "Hello, World!" << std::endl;

    // camera
    double targetAspectRatio = 16.0 / 9.0;
    int32_t imgWidth = 400;
    auto imgHeight = static_cast<int32_t>(imgWidth / targetAspectRatio);
    double viewportHeight = 2.0;
    double viewportWidth = viewportHeight * (static_cast<double>(imgWidth) / imgHeight);
    auto cameraCenter = Eigen::Vector3d{0, 0, 0};
    double focalLength = 1.0;

    Eigen::Vector3d viewportU = Eigen::Vector3d{viewportWidth, 0, 0}; // "WIDTH"
    Eigen::Vector3d viewportV = Eigen::Vector3d{0, -viewportHeight, 0}; // "HEIGHT"

    auto pixelDeltaU = viewportU / imgWidth;
    auto pixelDeltaV = viewportV / imgHeight;

    Eigen::Vector3d viewportTopLeft = cameraCenter + Eigen::Vector3d{0, 0, -focalLength} +
                           Eigen::Vector3d{-viewportWidth / 2.0, viewportHeight / 2.0, 0.0};
    Eigen::Vector3d topLeftPixelWorldCoords = viewportTopLeft + (pixelDeltaU + pixelDeltaV) / 2.0;

    // create img
    std::vector<Eigen::Vector3d> pixels;
    for (int32_t row = 0; row < imgHeight; ++row) {
        std::cout << "\rScanlines remaining: " << (imgHeight - row) << ' ' << std::flush;
        for (int32_t col = 0; col < imgWidth; ++col) {
            Eigen::Vector3d pixelCenter = topLeftPixelWorldCoords + (row * pixelDeltaV) + (col * pixelDeltaU);
            Eigen::Vector3d rayDir = pixelCenter - cameraCenter;
            Ray ray{cameraCenter, rayDir};
            auto color = rayColor(ray);
            pixels.push_back(color);
        }
    }
    std::cout << "\rDone.                 \n";
    Image<Eigen::Vector3d> image{std::move(pixels), imgWidth, imgHeight, 3};
    image.writeToFile(std::filesystem::path{"./"});
    return 0;
}
