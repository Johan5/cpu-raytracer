#pragma once

#include "Eigen/Core"

class Ray {
public:
    Ray() = default;

    Ray(const Eigen::Vector3d &origin, const Eigen::Vector3d &direction) : _orig(origin), _dir(direction) {
    }

    const Eigen::Vector3d &getOrigin() const {
        return _orig;
    }

    /// NOT guaranteed to be normalized
    const Eigen::Vector3d &getDirection() const {
        return _dir;
    }

    Eigen::Vector3d interpolate(double t) const {
        return _orig + t * _dir;
    }

private:
    Eigen::Vector3d _orig = {0, 0, 0};
    Eigen::Vector3d _dir = {0, 0, 0};
};