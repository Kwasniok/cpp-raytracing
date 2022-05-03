/**
 * @file
 * @brief metal material
 */

#ifndef CPP_RAYTRACING_MATERIALS_METAL_HPP
#define CPP_RAYTRACING_MATERIALS_METAL_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple colored metal material
 */
class Metal : public Material {
  public:
    /** @brief initialize with parameters */
    Metal(const Color& color, const Scalar roughness)
        : color(color), roughness(roughness) {}
    /** @brief copy constructor */
    Metal(const Metal&) = default;
    /** @brief move constructor */
    Metal(Metal&&) = default;
    /** @brief copy operation */
    Metal& operator=(const Metal&) = default;
    /** @brief move operation */
    Metal& operator=(Metal&&) = default;
    virtual ~Metal() = default;

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        const Vec3 para = dot(record.normal, ray.direction()) * record.normal;
        const Vec3 ortho = ray.direction() - para;
        const Vec3 direction = reflect(ortho, para, roughness);
        return {Ray(record.point, direction), color};
    }

  private:
    /** @brief reflected ray */
    static inline Vec3 reflect(const Vec3 ortho, const Vec3 para,
                               const Scalar roughness) {
        return ortho - para + roughness * random_vector_in_unit_sphere();
    }

  public:
    /** @brief color of the metal surface */
    Color color = Colors::WHITE;
    /**
     * @brief roughness of the diffuse surface
     * note: `roughness=0.0...1.0`
     */
    Scalar roughness = 0.0;
};

} // namespace cpp_raytracing

#endif
