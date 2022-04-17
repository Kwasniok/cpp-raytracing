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
        const Vec3 para = dot(record.normal, ray.direction()) * record.normal;
        const Vec3 ortho = ray.direction() - para;
        const Vec3 direction = reflect(ortho, para, roughness);
        return {Ray(record.point, direction), color};
    }

  private:
    static inline Vec3 reflect(const Vec3 ortho, const Vec3 para,
                               const Scalar roughness) {
        return ortho - para + roughness * random_vector_in_unit_sphere();
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
        // note: This algorithm assumes vacuum to medium transitions and
        // vice versa
        //       only.
        const Scalar refraction_ratio = record.front_face
                                            ? (1.0 / index_of_refraction)
                                            : index_of_refraction;
        const Vec3 unit_direction = unit_vector(ray.direction());
        const auto cos_theta = -dot(record.normal, unit_direction);
        const auto sin_theta_squared = std::abs(1.0 - std::pow(cos_theta, 2));
        Vec3 para = -cos_theta * record.normal;
        Vec3 ortho = unit_direction - para;

        Vec3 direction;
        //  Snell's law
        const bool cannot_refract =
            std::pow(refraction_ratio, 2) * sin_theta_squared > 1.0;
        // Scklick approximation
        const bool ray_reflects =
            cannot_refract || reflectance(cos_theta, refraction_ratio) >
                                  random_scalar<0.0, 1.0>();
        if (ray_reflects) {
            direction = reflect(ortho, para);
        } else {
            direction = refract(ortho, record.normal, refraction_ratio);
        }
        return {Ray(record.point, direction), color};
    }

  private:
    static constexpr Vec3 reflect(const Vec3 ortho, const Vec3 para) {
        return ortho - para;
    }
    static constexpr Vec3 refract(Vec3 ortho, const Vec3 normal,
                                  const Scalar index_of_refraction) {

        ortho *= index_of_refraction;
        const Vec3 para =
            -std::sqrt(std::abs(1.0 - ortho.length_squared())) * normal;
        return ortho + para;
    }

    static Scalar reflectance(const Scalar cos_theta,
                              const Scalar index_of_refraction) {
        // Schlick approximation
        auto x = (1.0 - index_of_refraction) / (1.0 + index_of_refraction);
        x *= x;
        return x + (1.0 - x) * std::pow(1 - cos_theta, 5);
    }

  public:
    Color color = Colors::WHITE;
    Scalar index_of_refraction = 1.0;
};

} // namespace ray

#endif
