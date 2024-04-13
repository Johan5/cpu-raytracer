#pragma once

#include "Eigen/Core"
#include "vec.h"
#include "material.h"

#include <cassert>

class Ray {
public:
    Ray() = default;

    Ray(const Vec3d &origin, const Vec3d &direction) : _orig(origin), _dir(direction) {
    }

    const Vec3d &getOrigin() const {
        return _orig;
    }

    /// NOT guaranteed to be normalized
    const Vec3d &getDirection() const {
        return _dir;
    }

    Vec3d interpolate(double t) const {
        return _orig + t * _dir;
    }

private:
    Vec3d _orig = {0, 0, 0};
    Vec3d _dir = {0, 0, 0};
};

struct RayIntersection {
    /// outwardsNormal must be normalized (Why?)
    static bool hitFrontFace(const Vec3d &rayDirection, const Vec3d &outwardsNormal) {
        assert(std::abs(outwardsNormal.norm() - 1) < 0.001);
        return rayDirection.dot(outwardsNormal) < 0;
    }

    Ray _incomingRay;
    Material _material;
    Vec3d _impact;
    /// The impact normal is always against the direction of the ray.
    /// Guaranteed to be normalized
    Vec3d _impactNormal;
    double _rayT;
    bool _hitFrontFace;
};