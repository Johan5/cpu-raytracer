#pragma once

#include "Eigen/Core"

#include "ray.h"

#include <optional>
#include "game_object.h"

class Sphere : public GameObject {
public:
    Sphere() = default;

    Sphere(const Vec3d &centre, double r) : GameObject(centre), _r(r) {
    }

    const Vec3d& getCentre() const {
        return _pos;
    }

    double getRadius() const {
        return _r;
    }

    std::optional<RayIntersection> rayIntersection(const Ray &ray, Interval rayInterval) const override {
        Vec3d oc = _pos - ray.getOrigin();
        double a = ray.getDirection().squaredNorm();
        double h = ray.getDirection().dot(oc);
        double c = oc.squaredNorm() - _r * _r;
        double discriminant = h * h - a * c;

        if (discriminant < 0) {
            return std::nullopt;
        }

        double sqrtD = sqrt(discriminant);

        double t = (h - sqrtD) / a;
        if (!rayInterval.containsExclusive(t)) {
            t = (h + sqrtD) / a;
            if (!rayInterval.containsExclusive(t)) {
                return std::nullopt;
            }
        }

        Vec3d impact = ray.interpolate(t);
        Vec3d outwardsNormal = (impact - _pos).normalized();
        bool hitFrontFace = RayIntersection::hitFrontFace(ray.getDirection(), outwardsNormal);
        Vec3d impactNormal = hitFrontFace ? outwardsNormal : -outwardsNormal;
        return RayIntersection{impact, impactNormal, t, hitFrontFace};
    }

private:
    double _r = 0.0;
};