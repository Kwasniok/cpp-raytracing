/**
 * @file
 * @brief animators of entities
 */

#ifndef CPP_RAYTRACING_ANIMATORS_ANIMATOR_HPP
#define CPP_RAYTRACING_ANIMATORS_ANIMATOR_HPP

#include <sstream>

#include "../../values/identifier.hpp"
#include "../../values/scalar.hpp"
#include "../../values/tensor.hpp"

namespace cpp_raytracing {

template <Dimension DIMENSION>
class Entity;

/**
 * @brief animators update the current state of an entity based on a given time
 */
template <Dimension DIMENSION>
class Animator {
  public:
    /** @brief unique animator identifier */
    Identifier<class Animator> id;

    /** @brief default constructor */
    Animator() = default;

    /** @brief copy constructor */
    Animator(const Animator&) = default;

    /** @brief move constructor */
    Animator(Animator&&) = default;

    /** @brief copy assignment */
    Animator& operator=(const Animator&) = default;

    /** @brief move assignment */
    Animator& operator=(Animator&&) = default;

    virtual ~Animator() = default;

    /** @brief invoke update of entity for given time */
    virtual void update_for_time(const Scalar time,
                                 Entity<DIMENSION>* entity) = 0;

  protected:
    /** @brief internal: throw after bad cast attempt */
    inline static void
    throw_bad_entity_type(const char* type,
                          const Identifier<Entity<DIMENSION>>& id) {
        std::stringstream msg;
        msg << "Cannot animate entity due to unexpected type. Entity `" << id
            << "` must be of type " << type << ".";
        throw std::runtime_error(std::move(msg).str());
    }
};

/** @brief 3D entity animator interface */
using Animator3D = Animator<Dimension{3}>;

/** @brief default identifier for animators */
template <Dimension DIMENSION>
struct default_identifier<Animator<DIMENSION>> {
    /** @brief default identifier for animators */
    static constexpr const char* value = "animator";
};

} // namespace cpp_raytracing

#endif
