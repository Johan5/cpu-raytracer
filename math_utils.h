#pragma once

#include "vec.h"
#include "ray.h"

#include <numeric>

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

/// [0, 1)
inline double randomDouble() {
    return rand() / (RAND_MAX + 1.0);
}

/// [min, max)
inline double randomDouble(double min, double max) {
    return min + (max-min) * randomDouble();
}

/// [0, 1)
inline Vec3d randomVec() {
    return Vec3d{randomDouble(), randomDouble(), randomDouble()};
}

inline Vec3d randomVec(double min, double max) {
    return Vec3d{randomDouble(min, max), randomDouble(min, max), randomDouble(min, max)};
}

inline Vec3d calcRandomVecOnSphere() {
    auto vecInSphere = []() {
        while (true) {
            Vec3d v = randomVec(-1, 1);
            if (v.squaredNorm() < 1) {
                return v;
            }
        }
    };
    return vecInSphere().normalized();
}

inline Vec3d randomVecOnHemisphere(const Vec3d& normal) {
    Vec3d unitVec = calcRandomVecOnSphere();
    if (unitVec.dot(normal) > 0.0) {
        return unitVec;
    }
    return -unitVec;
}

inline Vec3d lambertianReflection(const RayIntersection& intersection) {
    // draw a unit-sphere around the point (impact + impactNormal), our reflection should target a random point on this
    // sphere
    Vec3d sphereCenter = intersection._impact + intersection._impactNormal;
    Vec3d randomVecOnSphere = calcRandomVecOnSphere();
    Vec3d target = sphereCenter + randomVecOnSphere;
    return target - intersection._impact;
}
