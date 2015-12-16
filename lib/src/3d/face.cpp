#include "mh/3d/face.h"

#include "Eigen/Geometry"

namespace mh
{

std::shared_ptr<const Vertex> Face::getVertex(size_t idx) const
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

std::shared_ptr<Vertex> Face::getVertex(size_t idx)
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

    normal += getVertex(0)->getNormal();
    normal += getVertex(1)->getNormal();
    normal += getVertex(2)->getNormal();

    normal /= 3;

    return normal;
}

Intersection<Face> Face::intersect(const Ray & ray, bool backfacing)
{
    // Möller-Trumbore ray-triangle intersection
    // Fast, Minimum Storage Ray/Triangle Intersection [Möller and Trumbore, 1997]
    
    Eigen::Vector3f edge1 =  getEdge(0);
    Eigen::Vector3f edge2 = -getEdge(2);

    Eigen::Vector3f P = ray.getDirection().cross(edge2);

    float det = edge1.transpose() * P;

    if (det > -MH_EPSILON && det < MH_EPSILON) return Intersection<Face>();

    float invDet = 1.0f / det;

    Eigen::Vector3f T = ray.getPosition() - getVertex(0)->getPosition();

    float u = T.transpose() * P;
    u *= invDet;

    if (u < -MH_EPSILON || u > (1.0f + MH_EPSILON)) return Intersection<Face>();
    Eigen::VectorXf Q = T.cross(edge1);

    float v = ray.getDirection().transpose() * Q;
    v *= invDet;

    if (v < -MH_EPSILON || u + v > (1.0f + MH_EPSILON)) return Intersection<Face>();

    float t = edge2.transpose() * Q;
    t *= invDet;

    if (t > MH_EPSILON) {
        return Intersection<Face>(true, t, this);
    }

    return Intersection<Face>();

}

} // namespace mh
