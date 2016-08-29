#include "mh/3d/transform.h"

namespace mh
{

Eigen::Affine3f transform_to_mtw(const Transform & transform)
{
    return static_cast<Eigen::Affine3f>(Eigen::Translation3f(transform.getPosition()))
            * Eigen::UniformScaling<float>(transform.getScale()) * transform.getRotation();
}

} // namespace mh
