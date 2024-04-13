#pragma once

#include "vec.h"
#include "ray.h"

#include <numeric>
#include <optional>

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline bool almostEq(double a, double b) {
    return std::abs(a - b) < 0.0001;
}

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

/// Returns a random unit vector on (x,y) plane disc
inline Vec3d calcRandomVecOnDisk() {
    while (true) {
        Vec3d v = Vec3d{randomDouble(-1, 1), randomDouble(-1, 1), 0.0};
        if (v.squaredNorm() < 1) {
            return v;
        }
    }
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

inline bool vecIsAlmostZero(const Vec3d &vec) {
    double e = 1e-8;
    return std::abs(vec.x()) < e && std::abs(vec.y()) < e && std::abs(vec.z()) < e;
}

inline std::optional<Vec3d> calcReflection(const Vec3d& v, const Vec3d& n, double materialFuzz) {
    Vec3d reflectionVec = v - 2 * v.dot(n) * n;
    if (materialFuzz > 0) {
        reflectionVec.normalize();
        reflectionVec += materialFuzz * calcRandomVecOnSphere();
    }
    bool reflectionAboveSurface = reflectionVec.dot(n) > 0;
    if (reflectionAboveSurface) {
        return reflectionVec;
    }
    return std::nullopt;
}

/// v and n need to be unit vectors
/// refractionQuotient is the refraction of material we come from divided by refraction of material we are entering
inline std::optional<Vec3d> calcRefraction(const Vec3d &v, const Vec3d &n, double refractionQuotient) {
    assert(almostEq(v.norm(), 1) && almostEq(n.norm(), 1));
    double cosTheta = std::min((-v).dot(n), 1.0);

    double sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    bool cannotRefract = refractionQuotient * sinTheta > 1.0;
    if (cannotRefract) {
        return std::nullopt;
    }

    auto schlickReflectance = [](double cosine, double refractionIdx) {
        auto r0 = (1 - refractionIdx) / (1 + refractionIdx);
        r0 = r0 * r0;
        double reflectance = r0 + (1 - r0) * std::pow((1 - cosine), 5);
        return reflectance;
    };
    if (schlickReflectance(cosTheta, refractionQuotient) > randomDouble()) {
        return std::nullopt;
    }

    Vec3d rOutPerp = refractionQuotient * (v + cosTheta * n);
    Vec3d rOutParallel = -sqrt(std::abs(1.0 - rOutPerp.squaredNorm())) * n;
    return rOutPerp + rOutParallel;
}

/// Returns nullopt if ray is absorbed
inline std::optional<Vec3d> calcNewDirection(const RayIntersection &intersection) {
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
            std::optional<Vec3d> reflection = calcReflection(v, n, intersection._material._metalFuzz);
            return reflection;
        }
        case ReflectanceType::Dielectric: {
            double refracIdx = intersection._hitFrontFace ? (1.0 / intersection._material._refractionIndex)
                                                          : intersection._material._refractionIndex;
            Vec3d v = intersection._incomingRay.getDirection().normalized();
            std::optional<Vec3d> refraction = calcRefraction(v, intersection._impactNormal, refracIdx);
            if (refraction.has_value()) {
                return refraction;
            }
            std::optional<Vec3d> reflection = calcReflection(v, intersection._impactNormal, 0);
            return reflection;
        }
    }
    assert(false);
    return Vec3d{0, 0, 0};
}
