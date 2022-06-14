/**
 * @file
 * @brief isotropic material
 */

#ifndef CPP_RAYTRACING_MATERIALS_ISOTROPIC_HPP
#define CPP_RAYTRACING_MATERIALS_ISOTROPIC_HPP

#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief simple isotropic material (e.g. for volumes)
 * @note Suitable for volume materials.
 */
class Isotropic : public Material {
  public:
    /** @brief color of the isotropic 'surface' */
    Color color = Colors::WHITE;

    virtual ~Isotropic() = default;

    virtual std::pair<Ray, Color> scatter(const HitRecord& record,
                                          const Ray& ray) const override {
        return {Ray(record.point, random_vector_in_unit_sphere()), color};
    }
};

} // namespace cpp_raytracing

#endif
