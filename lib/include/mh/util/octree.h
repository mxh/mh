#ifndef OCTREE_H
#define OCTREE_H 

#include "Eigen/Core"

namespace mh
{

template <class TScalar>
class Octree
{
public:
    typedef TScalar Scalar;

    Octree() :
        m_child_000(nullptr),
        m_child_001(nullptr),
        m_child_010(nullptr),
        m_child_011(nullptr),
        m_child_100(nullptr),
        m_child_101(nullptr),
        m_child_110(nullptr),
        m_child_111(nullptr),
        m_leaf(true) {}

    bool is_leaf() const { return m_leaf; }

protected:

private:
    std::unique_ptr<Octree> m_child_000;
    std::unique_ptr<Octree> m_child_001;
    std::unique_ptr<Octree> m_child_010;
    std::unique_ptr<Octree> m_child_011;
    std::unique_ptr<Octree> m_child_100;
    std::unique_ptr<Octree> m_child_101;
    std::unique_ptr<Octree> m_child_110;
    std::unique_ptr<Octree> m_child_111;

    Eigen::Matrix<Scalar, 3, 1> m_min;
    Eigen::Matrix<Scalar, 3, 1> m_max;

    bool m_leaf;

}; // class Octree

} // namespace mh

#endif /* OCTREE_H */
