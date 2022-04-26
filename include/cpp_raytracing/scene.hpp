/**
 * @file
 * @brief scene representation
 */

#ifndef CPP_RAYTRACING_SCENE_HPP
#define CPP_RAYTRACING_SCENE_HPP

#include <memory>
#include <vector>

#include "camera.hpp"
#include "hittable.hpp"

namespace ray {

/**
 * @brief Collection of hittable objects and a camera.
 */
class Scene : public Hittable {
  public:
    /** @brief initialize with active #camera */
    Scene(const Camera& camera) : camera(camera){};
    /** @brief move constructor */
    Scene(Scene&&) = default;
    virtual ~Scene() = default;

    /** @brief remove all objects. */
    inline void clear() { _hittables.clear(); }
    /** @brief add an object. */
    inline void add(std::unique_ptr<Hittable>&& hittable) {
        _hittables.push_back(std::move(hittable));
    }

    virtual HitRecord
    hit_record(const Ray& ray, const Scalar t_min = 0.0,
               const Scalar t_max = SCALAR_INF) const override;

  public:
    /** @brief active camera of the scene used for rendering */
    Camera camera;

  private:
    std::vector<std::unique_ptr<Hittable>> _hittables;
};

HitRecord Scene::hit_record(const Ray& ray, const Scalar t_min,
                            const Scalar t_max) const {
    HitRecord closest_record = {.t = SCALAR_INF};

    for (const auto& hittable : _hittables) {
        HitRecord record = hittable->hit_record(ray, t_min, t_max);
        if (record.t < closest_record.t) {
            closest_record = record;
        }
    }

    return closest_record;
}

} // namespace ray

#endif
