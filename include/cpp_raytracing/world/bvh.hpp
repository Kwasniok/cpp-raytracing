/**
 * @file
 * @brief bounding volume hierachy
 */

#ifndef CPP_RAYTRACING_BVH_HPP
#define CPP_RAYTRACING_BVH_HPP

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <vector>

#include "axis_aligned_bounding_box.hpp"
#include "entities/base.hpp"

namespace cpp_raytracing {

/**
 * @brief bounding volume hierachy (BVH) tree
 * @note Used to speedup hit detection for (potenially) complex geometry.
 * @note BVHTree takes non-owning references to the entities and is immediately
 *       invalidated if any of the entities is changed.
 */
template <Dimension DIMENSION>
class BVHTree {
  private:
    /** @brief node of BVH tree for bounded entities only */
    struct Node {
        using Iter = typename std::vector<const Entity<DIMENSION>*>::iterator;

        /** @note might be nullptr  */
        const Entity<DIMENSION>* value = nullptr;
        /** @brief left branch*/
        std::unique_ptr<Node> left;
        /** @brief right branch */
        std::unique_ptr<Node> right;
        /**
         * @brief boundary of both branches and the value
         * @note MUST be finite at all times!
         */
        AxisAlignedBoundingBox<DIMENSION> bounds{Vec3{}, Vec3{}};

        Node() = default;
        Node(const Iter first, const Iter last) {

            const auto span = std::distance(first, last);

            if (span == 0) {
                return;
            }
            if (span == 1) {
                value = *first;
                bounds = value->bounding_box().value();
            } else {
                const int axis = rand() % 3;
                const auto comp = pseudo_comparators.at(axis);
                std::sort(first, last, comp);

                // note: Since the list is separated into two disjoint segments
                // parallelization is save.
                // note: Dispatch subtree generation as tasks and wait for them.
                const auto mid = std::next(first, span / 2);
                if (first != mid) {
#pragma omp task shared(left)
                    { left = std::make_unique<Node>(first, mid); }
                }
                if (last != mid) {
#pragma omp task shared(right)
                    { right = std::make_unique<Node>(mid, last); }
                }
#pragma omp taskwait

                if (left && right) {
                    bounds = surrounding_box(left->bounds, right->bounds);
                } else if (left && !right) {
                    bounds = left->bounds;
                } else if (!left && right) {
                    bounds = right->bounds;
                } else {
                    // should never happen
                }
            }
        }
        /**
         * @brief calculates the intersection of the light ray with the entities
         *        of the node and its branches
         * @note `closest_record.t = infinity` signals no intersection.
         * @see BVHTree::hit_record
         * @note The interface is changed in comparison to Entity::hit_record in
         *       order to optimize throughput.
         */
        void hit_record(const Geometry<DIMENSION>& geometry,
                        const RaySegment<DIMENSION>& ray_segment,
                        const Scalar t_min,
                        HitRecord<DIMENSION>& closest_record) const {
            if (bounds.hit(ray_segment, t_min, ray_segment.t_max())) {
                if (value) {
                    HitRecord<DIMENSION> record =
                        value->hit_record(geometry, ray_segment, t_min);
                    if (record.t < closest_record.t) {
                        closest_record = record;
                    }
                }
                if (left) {
                    left->hit_record(geometry, ray_segment, t_min,
                                     closest_record);
                }
                if (right) {
                    right->hit_record(geometry, ray_segment, t_min,
                                      closest_record);
                }
            }
        }

        /**
         * @brief returns number of entities managed by this node and its
         *        branches
         */
        std::size_t size() const {
            std::size_t count = value != nullptr ? 1 : 0;
            if (left) {
                count += left->size();
            }
            if (right) {
                count += right->size();
            }
            return count;
        }
    };

  public:
    /**
     * @brief construct BVH tree with container of bounded and unbounded
     *         entities
     * @tparam Container container of smart pointers to entities
     * @note IMORTANT: The container must be unaltered for the entire lifetime
     *       of this tree! Any change of the container or its elements
     *       invalidates this tree.
     */
    template <typename Container>
    BVHTree(const Container& container) {
        std::vector<const Entity<DIMENSION>*> bounded_entities;

        for (const auto& e : container) {
            if (e->is_bounded()) {
                bounded_entities.push_back(e.get());
            } else {
                _unbounded_entities.push_back(e.get());
            }
        }

        // begin omp section with thread pool and dispatch tasks later inside
        // Node::Node

#pragma omp parallel sections shared(bounded_entities)
        { _root = Node(bounded_entities.begin(), bounded_entities.end()); }
    }

    /**
     * @brief calculates the intersection of the light ray with the entities of
     *        the tree
     * @returns eihter a defned HitRecord or sets HitRecord::t to
     * @see Entity::hit_record
     */
    HitRecord<DIMENSION> hit_record(const Geometry<DIMENSION>& geometry,
                                    const RaySegment<DIMENSION>& ray_segment,
                                    const Scalar t_min = 0.0) const {
        HitRecord<DIMENSION> closest_record = {.t = infinity};
        _root.hit_record(geometry, ray_segment, t_min, closest_record);
        for (auto& unbounded_entity : _unbounded_entities) {
            HitRecord<DIMENSION> record =
                unbounded_entity->hit_record(geometry, ray_segment, t_min);
            if (record.t < closest_record.t) {
                closest_record = record;
            }
        }
        return closest_record;
    }

    /** @brief returns a boundaring box off all entities */
    std::optional<AxisAlignedBoundingBox<DIMENSION>> bounding_box() const {
        if (_unbounded_entities.size() > 0) {
            return std::nullopt;
        }
        return _root.bounds;
    }

    /** @brief returns number of bounded entities managed by this tree */
    std::size_t size_bounded() const {
        return _root.size();
    }

    /** @brief returns number of unbounded entities managed by this tree */
    std::size_t size_unbounded() const {
        return _unbounded_entities.size();
    }

    /** @brief returns number of entities managed by this tree */
    std::size_t size() const {
        return size_bounded() + size_unbounded();
    }

  private:
    /**
     * @note entities must not be nullptr and bounded
     * @note strict weak ordering (required by std::sort)
     */
    static bool pseudo_comparator(const std::size_t axis,
                                  const Entity<DIMENSION>* e1,
                                  const Entity<DIMENSION>* e2) {
        auto const b1 = e1->bounding_box();
        auto const b2 = e2->bounding_box();
        if (b1 && b2) {
            return b1->min()[axis] < b2->min()[axis];
        }
        return false; // strict weak ordering
    }

    /** @note entities must not be nullptr and bounded */
    static bool pseudo_comparator_x(const Entity<DIMENSION>* e1,
                                    const Entity<DIMENSION>* e2) {
        return pseudo_comparator(0, e1, e2);
    }
    /** @note entities must not be nullptr and bounded */
    static bool pseudo_comparator_y(const Entity<DIMENSION>* e1,
                                    const Entity<DIMENSION>* e2) {
        return pseudo_comparator(1, e1, e2);
    }
    /** @note entities must not be nullptr and bounded */
    static bool pseudo_comparator_z(const Entity<DIMENSION>* e1,
                                    const Entity<DIMENSION>* e2) {
        return pseudo_comparator(2, e1, e2);
    }

    using pseudo_cmp_t = bool(const Entity<DIMENSION>* e1,
                              const Entity<DIMENSION>* e2);
    /** @note entities must not be nullptr and bounded */
    static constexpr std::array<pseudo_cmp_t*, 3> pseudo_comparators = {
        pseudo_comparator_x, pseudo_comparator_y, pseudo_comparator_z};

  private:
    std::vector<const Entity<DIMENSION>*> _unbounded_entities;
    Node _root;
};

/** @brief 3D BVH tree */
using BVHTree3D = BVHTree<Dimension{3}>;

} // namespace cpp_raytracing

#endif
