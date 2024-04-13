#pragma once

#include "Eigen/Core"
#include "ray.h"
#include "vec.h"
#include "interval.h"

#include <optional>

class GameObject {
public:
    GameObject() = default;

    explicit GameObject(const Vec3d &pos) : _pos(pos) {}

    virtual ~GameObject() = default;

    virtual std::optional<RayIntersection> rayIntersection(const Ray &ray, Interval t) const {
        return std::nullopt;
    }

protected:
    Vec3d _pos = {0, 0, 0};
};
