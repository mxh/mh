#ifndef BVH_H
#define BVH_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/face.h"
#include "mh/3d/mesh.h"

namespace mh
{

class BVH
{
public:
    static const int X = 0;
    static const int Y = 1;
    static const int Z = 2;

    void                  setLeft         (std::unique_ptr<BVH> left)  { m_left = std::move(left); }
          BVH *           getLeft         (void)                       { return m_left.get(); }
    const BVH *           getLeft         (void)                 const { return m_left.get(); }

    void                  setRight        (std::unique_ptr<BVH> right) { m_right = std::move(right); }
          BVH *           getRight        (void)                       { return m_right.get(); }
    const BVH *           getRight        (void)                 const { return m_right.get(); }

    void                  setParent       (BVH * parent)               { m_parent = parent; }
          BVH *           getParent       (void)                       { return m_parent; }
    const BVH *           getParent       (void)                 const { return m_parent; }

    void                  setFace         (Face * face)                { m_face = face; }
          Face *          getFace         (void)                       { return m_face; }
    const Face *          getFace         (void)                 const { return m_face; }

    Eigen::Vector3f       getMin          (void) const                 { return m_min; }
    void                  setMin          (Eigen::Vector3f min)        { m_min = min; }

    Eigen::Vector3f       getMax          (void) const                 { return m_max; }
    void                  setMax          (Eigen::Vector3f max)        { m_max = max; }

    bool                  isLeaf          (void) const                 { return !(m_left || m_right); }

protected:

private:
    Eigen::Vector3f      m_min;
    Eigen::Vector3f      m_max;

    std::unique_ptr<BVH> m_left;
    std::unique_ptr<BVH> m_right;
    BVH *                m_parent;
    Face *               m_face;

}; // class BVH

std::unique_ptr<BVH> constructBVHFromFace(Face * face, const Eigen::Affine3f & transform);
std::unique_ptr<BVH> constructBVHFromMesh(Mesh * mesh, const Eigen::Affine3f & transform);
std::unique_ptr<BVH> constructBVHFromSet(std::vector<std::unique_ptr<BVH> > & bvhs, int axis=0, BVH * parent=nullptr);

bool aabbIntersect(const BVH & a, const BVH & b);
bool intersect_face(const Face * a, const Face * b,
    const Eigen::Affine3f & a_transform=Eigen::Affine3f::Identity(), const Eigen::Affine3f & b_transform=Eigen::Affine3f::Identity());
bool intersect_bvh(const BVH * a, const BVH * b);

} // namespace mh

#endif /* BVH_H */
