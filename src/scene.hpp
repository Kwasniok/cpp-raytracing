#ifndef CPP_RAYTRACING_SCENE_H
#define CPP_RAYTRACING_SCENE_H

#include <memory>
#include <vector>

#include "hittable.hpp"

namespace ray {

class Scene : public Hittable {
  public:
    Scene(){};

    inline void clear() { _hittables.clear(); }
    inline void add(std::unique_ptr<Hittable>&& hittable) {
        _hittables.push_back(std::move(hittable));
    }

    virtual HitRecord
    hit_record(const Ray& ray, const Scalar t_min = 0.0,
               const Scalar t_max = SCALAR_INF) const override;

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
