/**
 * @file
 * @brief tensor extension for boost::numeric::odeint
 */

#ifndef CPP_RAYTRACING_TENSOR_BOOST_NUMERIC_ODEINT_HPP
#define CPP_RAYTRACING_TENSOR_BOOST_NUMERIC_ODEINT_HPP

#include <boost/numeric/odeint/algebra/norm_result_type.hpp>
#include <boost/numeric/odeint/algebra/vector_space_algebra.hpp>

#include "tensor.hpp"

/* ------ Vec2 ----- */

namespace boost { namespace numeric { namespace odeint {

/** @brief inf norm for Vec2 */
template <>
struct vector_space_norm_inf<cpp_raytracing::Vec2> {
    /** @brief type */
    using result_type = cpp_raytracing::Scalar;
    /** @brief call */
    cpp_raytracing::Scalar operator()(const cpp_raytracing::Vec2& v) const {
        using std::max;
        using std::abs;
        return max(abs(v.u()), abs(v.v()));
    }
};

/** @brief norm result type for Vec2 */
template <>
struct norm_result_type<cpp_raytracing::Vec2> {
    /** @brief type */
    using type = cpp_raytracing::Scalar;
};

}}} // namespace boost::numeric::odeint

/* ------ Vec3 ----- */

namespace boost { namespace numeric { namespace odeint {

/** @brief inf norm for Vec3 */
template <>
struct vector_space_norm_inf<cpp_raytracing::Vec3> {
    /** @brief type */
    using result_type = cpp_raytracing::Scalar;
    /** @brief call */
    cpp_raytracing::Scalar operator()(const cpp_raytracing::Vec3& v) const {
        using std::max;
        using std::abs;
        return max(max(abs(v.x()), abs(v.y())), abs(v.z()));
    }
};

/** @brief norm result type for Vec3 */
template <>
struct norm_result_type<cpp_raytracing::Vec3> {
    /** @brief type */
    using type = cpp_raytracing::Scalar;
};

}}} // namespace boost::numeric::odeint

/* ------ Vec6 ----- */

namespace boost { namespace numeric { namespace odeint {

/** @brief inf norm for Vec6 */
template <>
struct vector_space_norm_inf<cpp_raytracing::Vec6> {
    /** @brief type */
    using result_type = cpp_raytracing::Scalar;
    /** @brief call */
    cpp_raytracing::Scalar operator()(const cpp_raytracing::Vec3& v) const {
        using std::max;
        using std::abs;
        cpp_raytracing::Scalar res = -cpp_raytracing::infinity;
        for (unsigned int i = 0; i < 6; ++i) {
            res = max(res, abs(v[i]));
        }
        return res;
    }
};

/** @brief norm result type for Vec6 */
template <>
struct norm_result_type<cpp_raytracing::Vec6> {
    /** @brief type */
    using type = cpp_raytracing::Scalar;
};

}}} // namespace boost::numeric::odeint

#endif
