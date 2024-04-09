#pragma once

#include "Eigen/Core"

#include "ray.h"

#include <optional>

class Sphere {
public:
    Sphere() = default;

    Sphere(const Eigen::Vector3d &centre, double r) : _centre(centre), _r(r) {
    }

    const Eigen::Vector3d& getCentre() const {
        return _centre;
    }

    double getRadius() const {
        return _r;
    }

    std::optional<Eigen::Vector3d> rayIntersection(const Ray &ray) const {
        double a = ray.getDirection().dot(ray.getDirection());
        double b = (-2 * ray.getDirection()).dot(_centre - ray.getOrigin());
        double c = (_centre - ray.getOrigin()).dot(_centre - ray.getOrigin()) - _r * _r;

        auto discriminant = b * b - 4 * a * c;
        if (discriminant < 0) {
            return std::nullopt;
        } else {
            double t = (-b - sqrt(discriminant)) / (2.0 * a);
            return ray.interpolate(t);
        }
    }

private:
    Eigen::Vector3d _centre = {0, 0, 0};
    double _r = 0.0;
};