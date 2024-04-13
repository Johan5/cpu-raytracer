#pragma once

enum class ReflectanceType {
    Lambertian,
    Metal,
};

struct Material {
    ReflectanceType _reflectanceType = ReflectanceType::Lambertian;
    Vec3d _albedo = {1,1,1};
    /// If this is 0, metal surfaces reflect perfectly
    double _metalFuzz = 1.0;

};