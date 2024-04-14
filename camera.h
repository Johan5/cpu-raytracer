#pragma once

#include "game_object.h"
#include "vec.h"
#include "game_object_container.h"
#include "image.h"
#include "Eigen/Core"
#include "Eigen/Geometry"


#include <algorithm>
#include <execution>
#include <filesystem>
#include <cstdint>
#include <iostream>

struct CameraConfig {
    double _targetAspectRatio = 16.0 / 9.0;
    // imgHeight is inferred from aspect ratio
    int32_t _imgWidth = 400;
    double _verticalFieldOfView = 90;
    int32_t _samplesPerPixel = 100;
    int32_t _maxRayDepth = 50;

    Vec3d _lookFrom = {0, 0, 0};
    Vec3d _lookAt = {0, 0, -1};
    Vec3d _viewUp = {0, 1, 0}; // Camera-space "up" direction

    // Length at which everything will be exactly in focus
    double _focusDist = 1;
    // For items not in focus, a larger defocusAngle will mean less sharp
    double _defocusAngle = 0;
};

class Camera : public GameObject {
public:
    explicit Camera(const CameraConfig &conf) : GameObject(conf._lookFrom), _conf(conf) {
        assert(_conf._defocusAngle >= 0);
        _imgHeight = static_cast<int32_t>(_conf._imgWidth / _conf._targetAspectRatio);
        double vFov = degrees_to_radians(_conf._verticalFieldOfView);
        double halfHeight = _conf._focusDist * std::tan(vFov / 2.0);
        _viewportHeight = 2.0 * halfHeight;
        _viewportWidth = _viewportHeight * (static_cast<double>(_conf._imgWidth) / _imgHeight);

        _w = (_conf._lookFrom - _conf._lookAt).normalized();
        _u = _conf._viewUp.cross(_w).normalized();
        _v = _w.cross(_u);

        // viewport (0,0) is top-left
        Vec3d viewportU = _viewportWidth * _u;
        Vec3d viewportV = _viewportHeight * -_v;

        _pixDeltaU = viewportU / _conf._imgWidth;
        _pixDeltaV = viewportV / _imgHeight;

        Vec3d viewportTopLeft = _pos - (_conf._focusDist * _w) - (viewportU / 2) - (viewportV / 2);
        _pix00Pos = viewportTopLeft + (_pixDeltaU + _pixDeltaV) / 2.0;

        double defocusRadius = _conf._focusDist * std::tan(degrees_to_radians(_conf._defocusAngle) / 2);
        _defocusDiskU = _u * defocusRadius;
        _defocusDiskV = _v * defocusRadius;
    }

    /// Result is written to file
    void render(const GameObjectContainer &objectsToRender) const {
        // create img
        auto raytracePixel = [this, &objectsToRender](int32_t row, int32_t col) {
            Vec3d pixelCenter = _pix00Pos + (row * _pixDeltaV) + (col * _pixDeltaU);
            Vec3d color{0, 0, 0};
            for (int32_t i = 0; i < _conf._samplesPerPixel; ++i) {
                Ray ray = constructRay(row, col);
                int32_t bounceCounter = 0;
                color += rayColor(ray, objectsToRender, bounceCounter);
            }
            color /= _conf._samplesPerPixel;
            return color;
        };

//        Vec3d debug = raytracePixel(_imgHeight / 2, _conf._imgWidth / 2);
        Vec3d debug = raytracePixel(_imgHeight / 2, 0);

        std::vector<std::pair<int32_t, int32_t>> indices;
        for (int32_t row = 0; row < _imgHeight; ++row) {
            for (int32_t col = 0; col < _conf._imgWidth; ++col) {
                indices.emplace_back(row, col);
            }
        }

        int64_t pixelCount = _imgHeight *_conf._imgWidth;
        std::atomic<int64_t> progressCounter = 0;
        std::vector<Vec3d> pixels(pixelCount, Vec3d{});
        std::for_each(
                std::execution::par_unseq,
                std::begin(indices),
                std::end(indices),
                [&](std::pair<int32_t, int32_t> idx) {
                    if (idx.second == 0) {
                        double progress = static_cast<double>(progressCounter) / static_cast<double>(pixelCount);
                        std::cout << std::setprecision(2) << "Progress: " << 100 * progress << " %\n";
                    }
                    Vec3d color = raytracePixel(idx.first, idx.second);
                    pixels[idx.first * _conf._imgWidth + idx.second] = color;
                    progressCounter++;
                }
        );

        std::cout << "\rDone.                 \n";
        Image<Vec3d> image{std::move(pixels), _conf._imgWidth, _imgHeight, 3};
        image.writeToFile(std::filesystem::path{"./"});
    }

private:
    Vec3d rayColor(const Ray &ray, const GameObjectContainer &gameObjectContainer, int32_t &bouncesSoFar) const {
        if (bouncesSoFar == _conf._maxRayDepth) {
            return Vec3d{0, 0, 0};
        }

        std::optional<RayIntersection> impact = gameObjectContainer.calcFirstRayIntersection(ray,
                                                                                             Interval{0.001, INF});
        if (impact.has_value()) {
            std::optional<Vec3d> bounceDir = calcNewDirection(impact.value());
            if (bounceDir.has_value()) {
                Ray newRay{impact->_impact, bounceDir.value()};

                Vec3d attenuationColor = impact->_material._albedo;
                Vec3d recursiveColor = rayColor(newRay, gameObjectContainer, ++bouncesSoFar);
                Vec3d combinedColor = attenuationColor.cwiseProduct(recursiveColor);

                return combinedColor;
            } else {
                /// ray was absorbed
                return Vec3d{0, 0, 0};
            }
        }

        // Background color
        const auto blue = Vec3d{0.5, 0.7, 1.0};
        const auto white = Vec3d{1.0, 1.0, 1.0};
        auto dir = ray.getDirection().normalized();
        auto a = (dir.y() + 1.0) / 2.0;
        return (1.0 - a) * white + a * blue;
    }

    Ray constructRay(int32_t row, int32_t col) const {
        Vec3d randomPixelOffset = {randomDouble() - 0.5, randomDouble() - 0.5, 0};
        Vec3d offsetU = ((col + randomPixelOffset.x()) * _pixDeltaU);
        Vec3d offsetV = ((row + randomPixelOffset.y()) * _pixDeltaV);
        Vec3d focusPlaneTarget = _pix00Pos + offsetU + offsetV;
        Vec3d rayOrigin = _pos;
        if (_conf._defocusAngle > 0) {
            Vec3d randomOffset = calcRandomVecOnDisk();
            rayOrigin += randomOffset.x() * _defocusDiskU + randomOffset.y() * _defocusDiskV;
        }

        Vec3d rayDir = focusPlaneTarget - rayOrigin;
        Ray ray{rayOrigin, rayDir};
        return ray;
    }

    CameraConfig _conf;
    int32_t _imgHeight;
    double _viewportWidth;
    double _viewportHeight;

    // Camera frame basis vectors
    Vec3d _u; // right
    Vec3d _v; // up
    Vec3d _w; // back

    Vec3d _pixDeltaU;
    Vec3d _pixDeltaV;
    Vec3d _pix00Pos;
    Vec3d _defocusDiskU;
    Vec3d _defocusDiskV;
};