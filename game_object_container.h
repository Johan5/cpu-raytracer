#pragma once

#include "game_object.h"
#include "ray.h"

#include <vector>
#include <memory>

class GameObjectContainer {
public:

    void add(std::unique_ptr<GameObject>&& gameObject) {
        _gameObjects.emplace_back(std::move(gameObject));
    }

    std::optional<RayIntersection> calcFirstRayIntersection(const Ray& ray, Interval rayInterval) const {
        std::optional<RayIntersection> bestSoFar;
        for (const std::unique_ptr<GameObject>& object : _gameObjects) {
            std::optional<RayIntersection> intersection = object->rayIntersection(ray, rayInterval);
            if (intersection) {
                bestSoFar = intersection;
                rayInterval.setMax(bestSoFar.value()._rayT);
            }
        }
        return bestSoFar;
    }

private:
    std::vector<std::unique_ptr<GameObject>> _gameObjects;
};