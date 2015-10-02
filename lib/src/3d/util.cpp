#include "3d/util.h"

namespace lp
{

float getFittingSphereRadius(const std::vector<Eigen::Vector3f> & verts, Eigen::Vector3f center)
{
    float radius = 0.0f;
    for (unsigned i = 0; i < verts.size(); ++i)
    {
        radius = std::max(radius, (verts[i] - center).norm());
    }

    return radius;
}

} // namespace lp
