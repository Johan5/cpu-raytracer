#pragma once

#include "vec.h"
#include "ray.h"

#include <numeric>
#include <optional>

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
    return min + (max - min) * randomDouble();
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

inline Vec3d randomVecOnHemisphere(const Vec3d &normal) {
    Vec3d unitVec = calcRandomVecOnSphere();
    if (unitVec.dot(normal) > 0.0) {
        return unitVec;
    }
    return -unitVec;
}

inline bool vecIsAlmostZero(const Vec3d& vec) {
    double e = 1e-8;
    return std::abs(vec.x()) < e && std::abs(vec.y()) < e && std::abs(vec.z()) < e;
}

/// Returns nullopt if ray is absorbed
inline std::optional<Vec3d> calcReflection(const RayIntersection &intersection) {
    switch (intersection._material._reflectanceType) {
        case ReflectanceType::Lambertian: {
            // draw a unit-sphere around the point (impact + impactNormal), our reflection should target a random point on this
            // sphere
            Vec3d sphereCenter = intersection._impact + intersection._impactNormal;
            Vec3d randomVecOnSphere = calcRandomVecOnSphere();
            Vec3d target = sphereCenter + randomVecOnSphere;
            Vec3d scatter = target - intersection._impact;
            if (vecIsAlmostZero(scatter)) {
                return intersection._impactNormal; // this is very unlikely
            }
            return scatter;
        }
        case ReflectanceType::Metal: {
            Vec3d v = intersection._incomingRay.getDirection();
            Vec3d n = intersection._impactNormal;
            Vec3d reflectionVec = v - 2 * v.dot(n) * n ;
            if (intersection._material._metalFuzz > 0) {
                reflectionVec.normalize();
                reflectionVec += intersection._material._metalFuzz * calcRandomVecOnSphere();
            }
            bool reflectionAboveSurface = reflectionVec.dot(n) > 0;
            if (reflectionAboveSurface) {
                return reflectionVec;
            }
            return std::nullopt;
        }
    }
    assert(false);
    return Vec3d{0,0,0};
}
