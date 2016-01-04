#include "mh/3d/bvh.h"

#include <cmath>

namespace
{

float distanceToAABB(Eigen::Vector3f min, Eigen::Vector3f max, Eigen::Vector3f p)
{
    float dx = std::max(std::max(min(0) - p(0), 0.0f), p(0) - max(0));
    float dy = std::max(std::max(min(1) - p(1), 0.0f), p(1) - max(1));
    float dz = std::max(std::max(min(2) - p(2), 0.0f), p(2) - max(2));

    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

} // anonymous namespace

namespace mh
{

Intersection<BVH> BVH::aabbIntersect(const Ray & ray)
{
    float tmin = std::numeric_limits<float>::min();
    float tmax = std::numeric_limits<float>::max();

    if ((ray.getPosition()(0) > m_min(0) && ray.getPosition()(0) < m_max(0)) &&
        (ray.getPosition()(1) > m_min(1) && ray.getPosition()(1) < m_max(1)) &&
        (ray.getPosition()(2) > m_min(2) && ray.getPosition()(2) < m_max(2)))
    {
        return Intersection<BVH>(true, 0.0, this); 
    }

    if (ray.getDirection()(0) != 0.0)
    {
        float tx1 = (m_min(0) - ray.getPosition()(0)) / ray.getDirection()(0);
        float tx2 = (m_max(0) - ray.getPosition()(0)) / ray.getDirection()(0);

        tmin = std::max(tmin, std::min(tx1, tx2));
        tmax = std::min(tmax, std::max(tx1, tx2));
    }

    if (ray.getDirection()(1) != 0.0)
    {
        float ty1 = (m_min(1) - ray.getPosition()(1)) / ray.getDirection()(1);
        float ty2 = (m_max(1) - ray.getPosition()(1)) / ray.getDirection()(1);

        tmin = std::max(tmin, std::min(ty1, ty2));
        tmax = std::min(tmax, std::max(ty1, ty2));
    }

    if (ray.getDirection()(2) != 0.0)
    {
        float tz1 = (m_min(2) - ray.getPosition()(2)) / ray.getDirection()(2);
        float tz2 = (m_max(2) - ray.getPosition()(2)) / ray.getDirection()(2);

        tmin = std::max(tmin, std::min(tz1, tz2));
        tmax = std::min(tmax, std::max(tz1, tz2));
    }

    if (tmax > 0 && tmax >= tmin)
    {
        if (tmin > 1e-5)
        {
            return Intersection<BVH>(true, tmin, this);
        } else {
            return Intersection<BVH>(true, tmax, this);
        }
    }

    return Intersection<BVH>();
}

Intersection<Face> BVH::intersect(const Ray & ray, bool backfacing, int depth, Eigen::Vector3f * offset)
{
    Ray offsetRay;
    if (offset)
    {
        offsetRay.setPosition(ray.getPosition() + *offset);
    } else {
        offsetRay.setPosition(ray.getPosition());
    }
    offsetRay.setDirection(ray.getDirection());

    if (isLeaf())
    {
        return m_face->intersect(offsetRay, backfacing);
    }

    Intersection<BVH> leftAABBIntersection = m_left->aabbIntersect(offsetRay);
    Intersection<BVH> rightAABBIntersection = m_right->aabbIntersect(offsetRay);

    Intersection<Face> leftIntersection;
    Intersection<Face> rightIntersection;

    if (leftAABBIntersection)
    {
        leftIntersection = m_left->intersect(offsetRay, backfacing, depth + 1);
    }

    if (rightAABBIntersection)
    {
        rightIntersection = m_right->intersect(offsetRay, backfacing, depth + 1);
    }

    if (leftIntersection && rightIntersection)
    {
        return (leftIntersection.getT() < rightIntersection.getT()) ? leftIntersection : rightIntersection;
    } else if (leftIntersection)
    {
        return leftIntersection;
    } else if (rightIntersection)
    {
        return rightIntersection;
    }

    return Intersection<Face>();
}

Eigen::Vector3f BVH::getClosestPoint(Eigen::Vector3f point)
{
    if (isLeaf())
    {
        return m_face->getClosestPoint(point);
    }

    float lDist = distanceToAABB(m_left->getMin(), m_left->getMax(), point);
    float rDist = distanceToAABB(m_right->getMin(), m_right->getMax(), point);

    bool  leftSmallest    = lDist < rDist;
    auto  closestAABB     = leftSmallest ? m_left  : m_right;
    /*float closestAABBDist = leftSmallest ? lDist   : rDist;*/
    auto  otherAABB       = leftSmallest ? m_right : m_left;
    float otherAABBDist   = leftSmallest ? rDist   : lDist;

    Eigen::Vector3f closestPoint;
    float           dist;

    closestPoint = closestAABB->getClosestPoint(point);
    dist         = (point - closestPoint).norm();

    if (dist > otherAABBDist)
    {
        Eigen::Vector3f altClosestPoint = otherAABB->getClosestPoint(point);
        float altDist                   = (point - altClosestPoint).norm();

        if (altDist < dist)
        {
            closestPoint = altClosestPoint;
        }
    }

    return closestPoint;
}

std::shared_ptr<BVH> constructBVHFromFace(std::shared_ptr<Face> face)
{
    auto bvh = std::make_shared<BVH>();

    bvh->setFace(face);

    std::vector<Eigen::Vector3f> faceVerts;

    std::shared_ptr<HalfEdge> startingEdge = face->getHalfEdge();
    std::shared_ptr<HalfEdge> currentEdge = startingEdge;
    do
    {
        faceVerts.push_back(currentEdge->getVertex()->getPosition());
        currentEdge = currentEdge->getNext();
    } while (currentEdge != startingEdge);

    bvh->setMin(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)faceVerts.data(), faceVerts.size(), 3).colwise().minCoeff());
    bvh->setMax(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)faceVerts.data(), faceVerts.size(), 3).colwise().maxCoeff());

    return bvh;
}

std::shared_ptr<BVH> constructBVHFromSet(std::vector<std::shared_ptr<BVH> > bvhs, int axis)
{
    if (bvhs.size() == 0)
    {
        return nullptr;
    }

    if (bvhs.size() == 1)
    {
        return bvhs[0];
    }

    std::sort(bvhs.begin(), bvhs.end(), [axis](std::shared_ptr<BVH> a, std::shared_ptr<BVH> b) { return a->getMin()[axis] < b->getMin()[axis]; });
    std::vector<std::shared_ptr<BVH> > leftBVHs;
    std::vector<std::shared_ptr<BVH> > rightBVHs;
    for (unsigned i = 0; i < bvhs.size(); ++i)
    {
        if (i < (bvhs.size() / 2))
        {
            leftBVHs.push_back(bvhs[i]);
        } else {
            rightBVHs.push_back(bvhs[i]);
        }
    }

    auto bvh = std::make_shared<BVH>();
    bvh->setLeft  (constructBVHFromSet(leftBVHs,  ((axis + 1) % 3)));
    bvh->setRight (constructBVHFromSet(rightBVHs, ((axis + 1) % 3)));

    Eigen::Matrix<float, 2, 3> mins;
    mins.block(0, 0, 1, 3) = bvh->getLeft()->getMin().transpose();
    mins.block(1, 0, 1, 3) = bvh->getRight()->getMin().transpose();

    Eigen::Matrix<float, 2, 3> maxs;
    maxs.block(0, 0, 1, 3) = bvh->getLeft()->getMax().transpose();
    maxs.block(1, 0, 1, 3) = bvh->getRight()->getMax().transpose();

    bvh->setMin(mins.colwise().minCoeff());
    bvh->setMax(maxs.colwise().maxCoeff());

    return bvh;
}

} // namespace mh
