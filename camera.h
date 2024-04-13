#pragma once

#include "game_object.h"
#include "vec.h"
#include "game_object_container.h"
#include "image.h"

#include <filesystem>
#include <cstdint>
#include <iostream>

struct CameraConfig {
    double _targetAspectRatio = 16.0 / 9.0;
    // imgHeight is inferred from aspect ratio
    int32_t _imgWidth = 400;
    // viewportWidth is inferred from aspect ratio
    double _viewportHeight = 2.0;
    double _focalLength = 1.0;
    int32_t _samplesPerPixel = 100;
};

class Camera : public GameObject {
public:
    Camera(const Vec3d &pos, const CameraConfig &conf) : GameObject(pos), _conf(conf) {
        _imgHeight = static_cast<int32_t>(_conf._imgWidth / _conf._targetAspectRatio);
        _viewportWidth = _conf._viewportHeight * (static_cast<double>(_conf._imgWidth) / _imgHeight);
    }

    /// Result is written to file
    void render(const GameObjectContainer &objectsToRender) const {
        Vec3d viewportU = Vec3d{static_cast<double>(_viewportWidth), 0, 0}; // "WIDTH"
        Vec3d viewportV = Vec3d{0, -_conf._viewportHeight, 0}; // "HEIGHT"

        auto pixelDeltaU = viewportU / _conf._imgWidth;
        auto pixelDeltaV = viewportV / _imgHeight;

        Vec3d viewportTopLeft = _pos + Vec3d{0, 0, -_conf._focalLength} +
                                Vec3d{-_viewportWidth / 2.0, _conf._viewportHeight / 2.0, 0.0};
        Vec3d topLeftPixelWorldCoords = viewportTopLeft + (pixelDeltaU + pixelDeltaV) / 2.0;


        // create img
        std::vector<Vec3d> pixels;
        for (int32_t row = 0; row < _imgHeight; ++row) {
            std::cout << "\rScanlines remaining: " << (_imgHeight - row) << ' ' << std::flush;
            for (int32_t col = 0; col < _conf._imgWidth; ++col) {
                if (row == _imgHeight / 2 && col == _conf._imgWidth / 2) {
                    int asd =2;
                }

                Vec3d pixelCenter = topLeftPixelWorldCoords + (row * pixelDeltaV) + (col * pixelDeltaU);
                Vec3d color{0,0,0};
                for (int32_t i = 0; i < _conf._samplesPerPixel; ++i) {
                    Vec3d offsetU = (randomDouble() - 0.5) * pixelDeltaU;
                    Vec3d offsetV = (randomDouble() - 0.5) * pixelDeltaV;
                    Vec3d rayTarget = pixelCenter + offsetU + offsetV;
                    Vec3d rayDir = rayTarget - _pos;
                    Ray ray{_pos, rayDir};
                    int32_t bounceCounter = 0;
                    color += rayColor(ray, objectsToRender, bounceCounter);
                }
                color /= _conf._samplesPerPixel;

                pixels.push_back(color);
            }
        }
        std::cout << "\rDone.                 \n";
        Image<Vec3d> image{std::move(pixels), _conf._imgWidth, _imgHeight, 3};
        image.writeToFile(std::filesystem::path{"./"});
    }

private:
    Vec3d rayColor(const Ray &ray, const GameObjectContainer &gameObjectContainer, int32_t& bouncesSoFar) const {
        constexpr int32_t BOUNCE_LIMIT = 50;
        if (bouncesSoFar > 3) {
            int asd =2;
        }
        if (bouncesSoFar == BOUNCE_LIMIT) {
            return Vec3d{0,0,0};
        }
        const auto blue = Vec3d{0.5, 0.7, 1.0};
        const auto white = Vec3d{1.0, 1.0, 1.0};

        std::optional<RayIntersection> impact = gameObjectContainer.calcFirstRayIntersection(ray,
                                                                                             Interval{0.001, INF});
        if (impact.has_value() && impact->_hitFrontFace) {
            Vec3d bounceDir = lambertianReflection(impact.value());
            Ray newRay{impact->_impact, bounceDir};
            return rayColor(newRay, gameObjectContainer, ++bouncesSoFar) / 2.0;
        }

        auto dir = ray.getDirection().normalized();
        auto a = (dir.y() + 1.0) / 2.0;
        return (1.0 - a) * white + a * blue;
    }



    CameraConfig _conf;
    int32_t _imgHeight;
    int32_t _viewportWidth;
};