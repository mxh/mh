#include "mh/util/bvh.h"
#include "mh/ext/tritri.h"

#include <iostream>

namespace mh
{

std::unique_ptr<BVH> constructBVHFromFace(const Face * face, const Eigen::Affine3f & transform)
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

std::unique_ptr<BVH> constructBVHFromFace(const Face * face, const Eigen::Matrix4f & transform)
{
    auto bvh = std::make_unique<BVH>();
    bvh->setFace(face);

    std::vector<Eigen::Vector3f> verts;
    for (size_t i = 0; i < 3; ++i)
    {
        Eigen::Vector4f transformed_vert = transform * face->getVertex(i)->getPosition().homogeneous();
        transformed_vert /= transformed_vert(3);
        verts.push_back(transformed_vert.block<3, 1>(0, 0));
    }

    bvh->setMin(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)verts.data(), verts.size(), 3).colwise().minCoeff());
    bvh->setMax(Eigen::Map<Eigen::Matrix<float, -1, 3, Eigen::RowMajor> >
                ((float*)verts.data(), verts.size(), 3).colwise().maxCoeff());

    return bvh;
}

std::unique_ptr<BVH> constructBVHFromMesh(const Mesh * mesh, const Eigen::Affine3f & transform)
{
    std::vector<std::unique_ptr<BVH> > face_bvhs;
    for (size_t i = 0; i < mesh->getFaces().size(); ++i)
    {
        face_bvhs.push_back(constructBVHFromFace(mesh->getFaces()[i].get(), transform * transform_to_mtw(mesh->getTransform())));
    }

    return constructBVHFromSet(face_bvhs);
}

std::unique_ptr<BVH> constructBVHFromMesh(const Mesh * mesh, const Eigen::Matrix4f & transform)
{
    std::vector<std::unique_ptr<BVH> > face_bvhs;
    for (size_t i = 0; i < mesh->getFaces().size(); ++i)
    {
        face_bvhs.push_back(constructBVHFromFace(mesh->getFaces()[i].get(), transform * transform_to_mtw(mesh->getTransform()).matrix()));
    }

    return constructBVHFromSet(face_bvhs);
}

std::unique_ptr<BVH> constructBVHFromSet(std::vector<std::unique_ptr<BVH> > & bvhs, int axis, const BVH * parent)
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

bool aabbIntersect(const BVH & a, const BVH & b, const Eigen::Matrix4f & a_transform, const Eigen::Matrix4f & b_transform)
{
    const Eigen::Vector3f a_max = a.getMax();
    const Eigen::Vector3f b_max = b.getMax();
    const Eigen::Vector3f a_min = a.getMin();
    const Eigen::Vector3f b_min = b.getMin();

    Eigen::Matrix<float, 4, 8> transformed_corners_a;
    transformed_corners_a << a_max(0), a_max(0), a_max(0), a_max(0), a_min(0), a_min(0), a_min(0), a_min(0),
                             a_max(1), a_max(1), a_min(1), a_min(1), a_max(1), a_max(1), a_min(1), a_min(1), 
                             a_max(2), a_min(2), a_max(2), a_min(2), a_max(2), a_min(2), a_max(2), a_min(2),
                                    1,        1,        1,        1,        1,        1,        1,        1;
    transformed_corners_a = a_transform * transformed_corners_a;

    Eigen::Vector3f t_a_min = transformed_corners_a.block<3, 8>(0, 0).rowwise().minCoeff();
    Eigen::Vector3f t_a_max = transformed_corners_a.block<3, 8>(0, 0).rowwise().maxCoeff();

    Eigen::Matrix<float, 4, 8> transformed_corners_b;
    transformed_corners_b << b_max(0), b_max(0), b_max(0), b_max(0), b_min(0), b_min(0), b_min(0), b_min(0),
                             b_max(1), b_max(1), b_min(1), b_min(1), b_max(1), b_max(1), b_min(1), b_min(1), 
                             b_max(2), b_min(2), b_max(2), b_min(2), b_max(2), b_min(2), b_max(2), b_min(2),
                                    1,        1,        1,        1,        1,        1,        1,        1;
    transformed_corners_b = b_transform * transformed_corners_b;

    Eigen::Vector3f t_b_min = transformed_corners_b.block<3, 8>(0, 0).rowwise().minCoeff();
    Eigen::Vector3f t_b_max = transformed_corners_b.block<3, 8>(0, 0).rowwise().maxCoeff();

    /*Eigen::Matrix<float, 4, 8> transformed_corners_b;

    transformed_corners_a.col(0) = {a_max(0), a_max(1), a_max(2), 1.0f};
    transformed_corners_a.col(1) = {a_max(0), a_max(1), a_min(2), 1.0f};
    transformed_corners_a.col(2) = {a_max(0), a_min(1), a_max(2), 1.0f};
    transformed_corners_a.col(3) = {a_max(0), a_min(1), a_min(2), 1.0f};
    transformed_corners_a.col(4) = {a_min(0), a_max(1), a_max(2), 1.0f};
    transformed_corners_a.col(5) = {a_min(0), a_max(1), a_min(2), 1.0f};
    transformed_corners_a.col(6) = {a_min(0), a_min(1), a_max(2), 1.0f};
    transformed_corners_a.col(7) = {a_min(0), a_min(1), a_min(2), 1.0f};
    transformed_corners_a = a_transform * transformed_corners_a;

    Eigen::Vector3f t_a_min = transformed_corners_a.rowwise().minCoeff();
    Eigen::Vector3f t_a_max = transformed_corners_a.rowwise().maxCoeff();

    transformed_corners_b.col(0) = {b_max(0), b_max(1), b_max(2), 1.0f};
    transformed_corners_b.col(1) = {b_max(0), b_max(1), b_min(2), 1.0f};
    transformed_corners_b.col(2) = {b_max(0), b_min(1), b_max(2), 1.0f};
    transformed_corners_b.col(3) = {b_max(0), b_min(1), b_min(2), 1.0f};
    transformed_corners_b.col(4) = {b_min(0), b_max(1), b_max(2), 1.0f};
    transformed_corners_b.col(5) = {b_min(0), b_max(1), b_min(2), 1.0f};
    transformed_corners_b.col(6) = {b_min(0), b_min(1), b_max(2), 1.0f};
    transformed_corners_b.col(7) = {b_min(0), b_min(1), b_min(2), 1.0f};
    transformed_corners_b = b_transform * transformed_corners_b;

    Eigen::Vector3f t_b_min = transformed_corners_b.rowwise().minCoeff();
    Eigen::Vector3f t_b_max = transformed_corners_b.rowwise().maxCoeff();*/

    bool intersection = !((t_a_max.array() <= t_b_min.array()).any() |
                          (t_b_max.array() <= t_a_min.array()).any());

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

bool intersect_face(const Face * a, const Face * b, const Eigen::Matrix4f & a_transform, const Eigen::Matrix4f & b_transform)
{
    Eigen::Vector3f a_0 = (a_transform * a->getVertex(0)->getPosition().homogeneous()).eval().hnormalized();
    Eigen::Vector3f a_1 = (a_transform * a->getVertex(1)->getPosition().homogeneous()).eval().hnormalized();
    Eigen::Vector3f a_2 = (a_transform * a->getVertex(2)->getPosition().homogeneous()).eval().hnormalized();

    Eigen::Vector3f b_0 = (b_transform * b->getVertex(0)->getPosition().homogeneous()).eval().hnormalized();
    Eigen::Vector3f b_1 = (b_transform * b->getVertex(1)->getPosition().homogeneous()).eval().hnormalized();
    Eigen::Vector3f b_2 = (b_transform * b->getVertex(2)->getPosition().homogeneous()).eval().hnormalized();

    bool face_intersection = NoDivTriTriIsect(a_0.data(),
                                              a_1.data(),
                                              a_2.data(),
                                              b_0.data(),
                                              b_1.data(),
                                              b_2.data());

    return face_intersection;
}

bool intersect_bvh(const BVH * a, const BVH * b, const Eigen::Affine3f & a_transform, const Eigen::Affine3f & b_transform)
{
    if (!aabbIntersect(*a, *b))
    {
        return false;
    }

    if (a->isLeaf() && b->isLeaf()) return intersect_face(a->getFace(), b->getFace(), a_transform, b_transform);
    if (a->isLeaf()) return intersect_bvh(a, b->getLeft(), a_transform, b_transform) || intersect_bvh(a, b->getRight(), a_transform, b_transform);
    if (b->isLeaf()) return intersect_bvh(a->getLeft(), b, a_transform, b_transform) || intersect_bvh(a->getRight(), b, a_transform, b_transform);

    return intersect_bvh(a->getLeft(),  b->getLeft(),  a_transform, b_transform)
        || intersect_bvh(a->getRight(), b->getLeft(),  a_transform, b_transform)
        || intersect_bvh(a->getLeft(),  b->getRight(), a_transform, b_transform)
        || intersect_bvh(a->getRight(), b->getRight(), a_transform, b_transform);

    /*bool a_left_b_left_aabb_intersection   = aabbIntersect(*a->getLeft(),  *b->getLeft());
    bool a_right_b_left_aabb_intersection  = aabbIntersect(*a->getRight(), *b->getLeft());
    bool a_left_b_right_aabb_intersection  = aabbIntersect(*a->getLeft(),  *b->getRight());
    bool a_right_b_right_aabb_intersection = aabbIntersect(*a->getRight(), *b->getRight());

    bool intersection = false;
    if (a_left_b_left_aabb_intersection)   intersection |= intersect_bvh(a->getLeft(),  b->getLeft(), a_transform, b_transform);
    if (intersection) return true;
    if (a_right_b_left_aabb_intersection)  intersection |= intersect_bvh(a->getRight(), b->getLeft(), a_transform, b_transform);
    if (intersection) return true;
    if (a_left_b_right_aabb_intersection)  intersection |= intersect_bvh(a->getLeft(),  b->getRight(), a_transform, b_transform);
    if (intersection) return true;
    if (a_right_b_right_aabb_intersection) intersection |= intersect_bvh(a->getRight(), b->getRight(), a_transform, b_transform);
    return intersection;*/
}

bool intersect_bvh(const BVH * a, const BVH * b, const Eigen::Matrix4f & a_transform, const Eigen::Matrix4f & b_transform)
{
    if (!aabbIntersect(*a, *b, a_transform, b_transform))
    {
        return false;
    }

    if (a->isLeaf() && b->isLeaf()) return intersect_face(a->getFace(), b->getFace(), a_transform, b_transform);
    if (a->isLeaf()) return intersect_bvh(a, b->getLeft(), a_transform, b_transform) || intersect_bvh(a, b->getRight(), a_transform, b_transform);
    if (b->isLeaf()) return intersect_bvh(a->getLeft(), b, a_transform, b_transform) || intersect_bvh(a->getRight(), b, a_transform, b_transform);

    return intersect_bvh(a->getLeft(),  b->getLeft(),  a_transform, b_transform)
        || intersect_bvh(a->getRight(), b->getLeft(),  a_transform, b_transform)
        || intersect_bvh(a->getLeft(),  b->getRight(), a_transform, b_transform)
        || intersect_bvh(a->getRight(), b->getRight(), a_transform, b_transform);

    /*bool a_left_b_left_aabb_intersection   = aabbIntersect(*a->getLeft(),  *b->getLeft());
    bool a_right_b_left_aabb_intersection  = aabbIntersect(*a->getRight(), *b->getLeft());
    bool a_left_b_right_aabb_intersection  = aabbIntersect(*a->getLeft(),  *b->getRight());
    bool a_right_b_right_aabb_intersection = aabbIntersect(*a->getRight(), *b->getRight());

    bool intersection = false;
    if (a_left_b_left_aabb_intersection)   intersection |= intersect_bvh(a->getLeft(),  b->getLeft(), a_transform, b_transform);
    if (intersection) return true;
    if (a_right_b_left_aabb_intersection)  intersection |= intersect_bvh(a->getRight(), b->getLeft(), a_transform, b_transform);
    if (intersection) return true;
    if (a_left_b_right_aabb_intersection)  intersection |= intersect_bvh(a->getLeft(),  b->getRight(), a_transform, b_transform);
    if (intersection) return true;
    if (a_right_b_right_aabb_intersection) intersection |= intersect_bvh(a->getRight(), b->getRight(), a_transform, b_transform);
    return intersection;*/
}

} // namespace mh
