/**
 * @file
 * @brief hittable mesh object
 */

#ifndef CPP_RAYTRACING_ENTITIES_MESH_HPP
#define CPP_RAYTRACING_ENTITIES_MESH_HPP

#include <array>
#include <memory>
#include <vector>

#include "../../values/tensor.hpp"
#include "../materials/base.hpp"
#include "base.hpp"

namespace cpp_raytracing {

/**
 * @brief mesh object
 * @note Each face is filled via bilinear coordinate interpolation.
 */
template <Dimension DIMENSION>
class Mesh : public Entity<DIMENSION> {
  public:
    /** @brief position vector type */
    using VolumeVec = Vec<DIMENSION>;
    /** @brief point index type */
    using Index = typename std::vector<VolumeVec>::size_type;
    /** @brief face type (triplet of point indices)*/
    using Face = std::array<Index, 3>;

    /** @brief face corners */
    std::vector<VolumeVec> points;
    /** @brief faces */
    std::vector<Face> faces;
    /** @brief material of the plane */
    std::shared_ptr<Material<DIMENSION>> material;

    /** @brief default constructor */
    Mesh() = default;

    /** @brief copy constructor */
    Mesh(const Mesh&) = delete;

    /** @brief move constructor */
    Mesh(Mesh&&) = default;

    /** @brief copy assignment */
    Mesh& operator=(const Mesh&) = delete;

    /** @brief move assignment */
    Mesh& operator=(Mesh&&) = default;

    ~Mesh() override = default;

    /** @see Entity::set_time */
    void set_time(const Scalar time) override {
        Entity<DIMENSION>::set_time(time);
        generate_cache();
    }

    /** @see Entity::bounding_box */
    std::optional<AxisAlignedBoundingBox<DIMENSION>>
    bounding_box() const override {
        return {_bounds};
    }

  private:
    void generate_cache() {

        constexpr Scalar epsilon = 1e-8;

        VolumeVec low = {+infinity, +infinity, +infinity};
        VolumeVec high = {-infinity, -infinity, -infinity};

        for (const auto& point : points) {
            low.inplace_elementwise(min, point);
            high.inplace_elementwise(max, point);
        }

        // padding to guarantee non-zero volume
        low -= low.elementwise(abs) * epsilon;
        high += high.elementwise(abs) * epsilon;

        for (unsigned int i = 0; i < 3; ++i) {
            if (low[i] == high[i]) {
                low[i] -= epsilon;
                high[i] += epsilon;
            }
        }

        _bounds = {low, high};
    }

  private:
    // note: initial value is irrelevant
    AxisAlignedBoundingBox<DIMENSION> _bounds = {
        tensor::zero_vec<DIMENSION>,
        tensor::zero_vec<DIMENSION>,
    };
};

/**
 * @brief mesh object in a 3D maifold
 * @see Mesh
 */
using Mesh3D = Mesh<Dimension{3}>;

/**
 * @brief face of a 3D mesh object (triplet of point indices)
 * @see Mesh::Face
 */
using Face3D = Mesh3D::Face;

} // namespace cpp_raytracing

#endif
