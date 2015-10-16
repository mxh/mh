#ifndef UTIL_H
#define UTIL_H 

#include <vector>

#include "eigen3/Eigen/Core"

namespace mh
{

template <class TVecType, class TTransformType>
std::vector<TVecType> applyTransform         (TTransformType T, const std::vector<TVecType> & data);

float                 getFittingSphereRadius (const std::vector<Eigen::Vector3f> & points, Eigen::Vector3f center);

} // namespace mh

#include "impl/util.hpp"

#endif /* UTIL_H */
