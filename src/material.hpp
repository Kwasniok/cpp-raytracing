#ifndef CPP_RAYTRACING_SHADER_H
#define CPP_RAYTRACING_SHADER_H

#include <utility>

#include "color.hpp"
#include "hittable.hpp"
#include "ray.hpp"
#include "scalar.hpp"
#include "vec3.hpp"

namespace ray {

class Material {

  public:
    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const = 0;
};

class Diffuse : public Material {
  public:
    Diffuse(const Color& color) : color(color) {}

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        Vec3 direction = record.normal + random_unit_vector();
        if (direction.near_zero(1.0e-12)) {
            // in case of normal and random vector beeing antiparallel
            // use normal instead
            direction = record.normal;
        }
        return {Ray(record.point, direction), color};
    }

  public:
    Color color = Colors::WHITE;
};

class Metal : public Material {
  public:
    Metal(const Color& color, const Scalar roughness)
        : color(color), roughness(roughness) {}

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        const Vec3 ray_para =
            dot(record.normal, ray.direction()) * record.normal;
        const Vec3 ray_ortho = ray.direction() - ray_para;
        const Vec3 direction =
            ray_ortho - ray_para + roughness * random_vector_in_unit_sphere();
        return {Ray(record.point, direction), color};
    }

  public:
    Color color = Colors::WHITE;
    Scalar roughness = 0.0;
};

class Dielectric : public Material {
  public:
    Dielectric(const Color& color, const Scalar index_of_refraction)
        : color(color), index_of_refraction(index_of_refraction) {}

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        // note: This algorithm assumes vacuum to medium transitions and vice
        // versa
        //       only.
        const Scalar refraction_ratio = record.front_face
                                            ? (1.0 / index_of_refraction)
                                            : index_of_refraction;
        const Vec3 unit_direction = unit_vector(ray.direction());
        const auto cos_theta = -dot(record.normal, unit_direction);
        const auto sin_theta_squared = std::abs(1.0 - std::pow(cos_theta, 2));
        Vec3 ray_para = -cos_theta * record.normal;
        Vec3 ray_ortho = unit_direction - ray_para;

        Vec3 direction;
        const bool cannot_refract =
            std::pow(refraction_ratio, 2) * sin_theta_squared > 1.0;
        if (cannot_refract) {
            // total reflection
            direction = ray_ortho - ray_para;
        } else {
            // refraction
            ray_ortho *= refraction_ratio;
            ray_para = -std::sqrt(std::abs(1.0 - ray_ortho.length_squared())) *
                       record.normal;
            direction = ray_ortho + ray_para;
        }
        return {Ray(record.point, direction), color};
    }

  public:
    Color color = Colors::WHITE;
    Scalar index_of_refraction = 1.0;
};

} // namespace ray

#endif
