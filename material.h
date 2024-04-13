#pragma once

enum class ReflectanceType {
    Lambertian,
    Metal,
    Dielectric,
};

struct Material {
    ReflectanceType _reflectanceType = ReflectanceType::Lambertian;
    Vec3d _albedo = {1,1,1};
    /// If this is 0, metal surfaces reflect perfectly
    double _metalFuzz = 1.0;
    /// Ratio of the material's refraction index over refraction index of enclosing media
    double _refractionIndex = 1.0;
};