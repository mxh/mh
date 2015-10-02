#ifndef UTIL_H
#define UTIL_H 

#include <vector>

#include "eigen3/Eigen/Core"

namespace lp
{

template <class _VecType, class _TransformType>
std::vector<_VecType> applyTransform         (_TransformType T, const std::vector<_VecType> & data);

float                 getFittingSphereRadius (const std::vector<Eigen::Vector3f> & points, Eigen::Vector3f center);

} // namespace lp

#include "impl/util.hpp"

#endif /* UTIL_H */
