#include "mh/util/bvh.h"
#include "mh/ext/tritri.h"

#include <iostream>

namespace mh
{

std::unique_ptr<BVH> constructBVHFromFace(Face * face, const Eigen::Affine3f & transform)
{
    auto bvh = std::make_unique<BVH>();
    bvh->setFace(face);

    std::vector<Eigen::Vector3f> verts;
    for (size_t i = 0; i < 3; ++i) verts.push_back(transform * face->getVertex(i)->getPosition());

    bvh->setMin(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)verts.data(), verts.size(), 3).colwise().minCoeff());
    bvh->setMax(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)verts.data(), verts.size(), 3).colwise().maxCoeff());

    return bvh;
}

std::unique_ptr<BVH> constructBVHFromMesh(Mesh * mesh, const Eigen::Affine3f & transform)
{
    std::vector<std::unique_ptr<BVH> > face_bvhs;
    for (size_t i = 0; i < mesh->getFaces().size(); ++i)
    {
        face_bvhs.push_back(constructBVHFromFace(mesh->getFaces()[i].get(), transform * transform_to_mtw(mesh->getTransform())));
    }

    return constructBVHFromSet(face_bvhs);
}

std::unique_ptr<BVH> constructBVHFromSet(std::vector<std::unique_ptr<BVH> > & bvhs, int axis, BVH * parent)
{
    if (bvhs.size() == 0)
    {
        return nullptr;
    }

    if (bvhs.size() == 1)
    {
        return std::move(bvhs[0]);
    }

    std::sort(bvhs.begin(), bvhs.end(), [axis](const std::unique_ptr<BVH> & a, const std::unique_ptr<BVH> & b) { return a->getMin()[axis] < b->getMin()[axis]; });
    std::vector<std::unique_ptr<BVH> > leftBVHs;
    std::vector<std::unique_ptr<BVH> > rightBVHs;
    for (unsigned i = 0; i < bvhs.size(); ++i)
    {
        if (i < (bvhs.size() / 2))
        {
            leftBVHs.push_back(std::move(bvhs[i]));
        } else {
            rightBVHs.push_back(std::move(bvhs[i]));
        }
    }

    auto bvh = std::make_unique<BVH>();
    bvh->setParent(parent);
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

bool aabbIntersect(const BVH & a, const BVH & b)
{
    bool intersection = !((a.getMax().array() <= b.getMin().array()).any() |
                          (b.getMax().array() <= a.getMin().array()).any());

    return intersection;
}

bool intersect_face(const Face * a, const Face * b, const Eigen::Affine3f & a_transform, const Eigen::Affine3f & b_transform)
{
    Eigen::Vector3f a_0 = a_transform * a->getVertex(0)->getPosition();
    Eigen::Vector3f a_1 = a_transform * a->getVertex(1)->getPosition();
    Eigen::Vector3f a_2 = a_transform * a->getVertex(2)->getPosition();

    Eigen::Vector3f b_0 = b_transform * b->getVertex(0)->getPosition();
    Eigen::Vector3f b_1 = b_transform * b->getVertex(1)->getPosition();
    Eigen::Vector3f b_2 = b_transform * b->getVertex(2)->getPosition();

    bool face_intersection = NoDivTriTriIsect(a_0.data(),
                                              a_1.data(),
                                              a_2.data(),
                                              b_0.data(),
                                              b_1.data(),
                                              b_2.data());

    return face_intersection;
}

bool intersect_bvh(const BVH * a, const BVH * b)
{
    if (!aabbIntersect(*a, *b)) return false;

    if (a->isLeaf() && b->isLeaf()) return intersect_face(a->getFace(), b->getFace());
    if (a->isLeaf()) return intersect_bvh(a, b->getLeft()) || intersect_bvh(a, b->getRight());
    if (b->isLeaf()) return intersect_bvh(a->getLeft(), b) || intersect_bvh(a->getRight(), b);

    bool a_left_b_left_aabb_intersection   = aabbIntersect(*a->getLeft(),  *b->getLeft());
    bool a_right_b_left_aabb_intersection  = aabbIntersect(*a->getRight(), *b->getLeft());
    bool a_left_b_right_aabb_intersection  = aabbIntersect(*a->getLeft(),  *b->getRight());
    bool a_right_b_right_aabb_intersection = aabbIntersect(*a->getRight(), *b->getRight());

    bool intersection = false;
    if (a_left_b_left_aabb_intersection)   intersection |= intersect_bvh(a->getLeft(),  b->getLeft());
    if (intersection) return true;
    if (a_right_b_left_aabb_intersection)  intersection |= intersect_bvh(a->getRight(), b->getLeft());
    if (intersection) return true;
    if (a_left_b_right_aabb_intersection)  intersection |= intersect_bvh(a->getLeft(),  b->getRight());
    if (intersection) return true;
    if (a_right_b_right_aabb_intersection) intersection |= intersect_bvh(a->getRight(), b->getRight());
    return intersection;
}

} // namespace mh
