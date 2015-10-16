#ifndef BVH_H
#define BVH_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/face.h"
#include "mh/3d/intersection.h"
#include "mh/3d/ray.h"

namespace mh
{

class BVH
{
public:
    static const int X = 0;
    static const int Y = 1;
    static const int Z = 2;

    void                  setLeft       (std::shared_ptr<BVH> left)  { m_left = left; }
    std::shared_ptr<BVH>  getLeft       (void)                       { return m_left; }
    void                  setRight      (std::shared_ptr<BVH> right) { m_right = right; }
    std::shared_ptr<BVH>  getRight      (void)                       { return m_right; }

    void                  setFace       (std::shared_ptr<Face> face) { m_face = face; }
    std::shared_ptr<Face> getFace       (void)                       { return m_face; }

    Eigen::Vector3f       getMin        (void) const                 { return m_min; }
    void                  setMin        (Eigen::Vector3f min)        { m_min = min; }
    Eigen::Vector3f       getMax        (void)                       { return m_max; }
    void                  setMax        (Eigen::Vector3f max)        { m_max = max; }

    bool                  isLeaf        (void)                       { return !(m_left || m_right); }

    Intersection<BVH>     aabbIntersect (const Ray & ray);
    Intersection<Face>    intersect     (const Ray & ray, bool backfacing=false);
    
protected:

private:
    Eigen::Vector3f       m_min;
    Eigen::Vector3f       m_max;

    std::shared_ptr<BVH>  m_left;
    std::shared_ptr<BVH>  m_right;
    std::shared_ptr<Face> m_face;

}; // class BVH

std::shared_ptr<BVH> constructBVHFromFace (std::shared_ptr<Face> face);
std::shared_ptr<BVH> constructBVHFromSet  (std::vector<std::shared_ptr<BVH> > bvhs, int axis);

} // namespace mh

#endif /* BVH_H */
