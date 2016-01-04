#include "mh/3d/face.h"

#include "Eigen/Geometry"

namespace
{

using namespace mh;

void getBarycentricCoordsOfProjection(Eigen::Vector3f p, const Face & face, float & alpha, float & beta, float & gamma)
{
    Eigen::Vector3f A = face.getVertex(0)->getPosition();
    Eigen::Vector3f B = face.getVertex(1)->getPosition();
    Eigen::Vector3f C = face.getVertex(2)->getPosition();

    Eigen::Vector3f u = B - A;
    Eigen::Vector3f v = C - A;

    Eigen::Vector3f n = u.cross(v);

    Eigen::Vector3f w = p - A;

    gamma = ((u.cross(w).transpose() * n) / n.squaredNorm()).value();
    beta  = ((w.cross(v).transpose() * n) / n.squaredNorm()).value();
    alpha = 1 - (gamma + beta);
}

} // internal namespace

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

Eigen::Vector3f Face::getClosestPoint(Eigen::Vector3f point)
{
    // find barycentric coordinates
    float alpha, beta, gamma;
    getBarycentricCoordsOfProjection(point, *this, alpha, beta, gamma);

    // 7 different cases
    
    // point is inside triangle
    if (alpha >= 0.0f && alpha <= 1.0f &&
        beta  >= 0.0f && beta  <= 1.0f &&
        gamma >= 0.0f && gamma <= 1.0f)
    {
        return alpha * getVertex(0)->getPosition() +
               beta  * getVertex(1)->getPosition() +
               gamma * getVertex(2)->getPosition();
    }

    // closest to BC
    if (beta >= 0.0f && gamma >= 0.0f && (beta + gamma >= 1.0f))
    {
        float scale = beta + gamma;
        beta  /= scale;
        gamma /= scale;
        alpha = 1 - (beta + gamma);

        return alpha * getVertex(0)->getPosition() +
               beta  * getVertex(1)->getPosition() +
               gamma * getVertex(2)->getPosition();
    }

    // closest to AC
    if (alpha >= 0.0f && gamma >= 0.0f && (alpha + gamma >= 1.0f))
    {
        float scale = alpha + gamma;
        alpha /= scale;
        gamma /= scale;
        beta = 1 - (alpha + gamma);

        return alpha * getVertex(0)->getPosition() +
               beta  * getVertex(1)->getPosition() +
               gamma * getVertex(2)->getPosition();
    }

    // closest to AB
    if (alpha >= 0.0f && beta >= 0.0f && (alpha + beta >= 1.0f))
    {
        float scale = alpha + beta;
        alpha /= scale;
        beta  /= scale;
        gamma = 1 - (alpha + beta);
        
        return alpha * getVertex(0)->getPosition() +
               beta  * getVertex(1)->getPosition() +
               gamma * getVertex(2)->getPosition();
    }

    // closest to A
    if (beta < 0.0f && alpha > 1.0f && (beta + alpha >= 1.0f))
    {
        return getVertex(0)->getPosition();
    }

    // closest to B
    if (gamma < 0.0f && beta > 0.0f && (beta + gamma >= 1.0f))
    {
        return getVertex(1)->getPosition();
    }

    // closest to C
    if (beta < 0.0f && gamma > 1.0f && (beta + gamma >= 1.0f))
    {
        return getVertex(2)->getPosition();
    }

    // should not reach here!
    return Eigen::Vector3f::Zero();

}

} // namespace mh
