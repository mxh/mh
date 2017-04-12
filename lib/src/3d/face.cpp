#include "mh/3d/face.h"

#include "Eigen/Geometry"

namespace mh
{

const Vertex * Face::getVertex(size_t idx) const
{
    MH_ASSERT(idx < 3); // we deal only in triangles in these parts

    switch (idx)
    {
        case 0:
            return getHalfEdge()->getVertex();
            break;
        case 1:
            return getHalfEdge()->getNext()->getVertex();
            break;
        case 2:
            return getHalfEdge()->getNext()->getNext()->getVertex();
            break;
    }

    return nullptr;
}

Vertex * Face::getVertex(size_t idx)
{
    MH_ASSERT(idx < 3); // we deal only in triangles in these parts

    switch (idx)
    {
        case 0:
            return getHalfEdge()->getVertex();
            break;
        case 1:
            return getHalfEdge()->getNext()->getVertex();
            break;
        case 2:
            return getHalfEdge()->getNext()->getNext()->getVertex();
            break;
    }

    return nullptr;
}

Eigen::Vector3f Face::getEdge(size_t idx) const
{
    MH_ASSERT(idx < 3); // we deal only in triangles in these parts

    switch (idx)
    {
        case 0:
            return getVertex(1)->getPosition() - getVertex(0)->getPosition();
            break;
        case 1:
            return getVertex(2)->getPosition() - getVertex(1)->getPosition();
            break;
        case 2:
            return getVertex(0)->getPosition() - getVertex(2)->getPosition();
            break;
    }

    return Eigen::Vector3f::Zero();

}

Eigen::Vector3f Face::getFaceNormal(void) const
{
    Eigen::Vector3f normal(0.0f, 0.0f, 0.0f);

    normal += m_wedges[0]->getNormal();
    normal += m_wedges[1]->getNormal();
    normal += m_wedges[2]->getNormal();

    normal /= 3;

    return normal;
}

} // namespace mh
