#ifndef KD_TREE_H
#define KD_TREE_H 

namespace mh
{

template <class TNodeType> class KDTreeTraits;

template <class TNodeType>
class KDTree
{
public:
    typedef TNodeType NodeType;
    typedef KDTreeTraits<TNodeType>::DimType DimType;

    KDTree(DimType splitting_dim,
           NodeType * node=nullptr,
           KDTree * parent=nullptr,
           std::unique_ptr<KDTree> left_child=nullptr,
           std::unique_ptr<KDTree> right_child=nullptr)
       : m_splitting_dim(splitting_dim),
         m_node(node),
         m_parent(parent),
         m_left(nullptr),
         m_right(nullptr) {}

    void set_left_child(std::unique_ptr<KDTree> child) { m_left = std::move(child); }
    void set_right_child(std::unique_ptr<KDTree> child) { m_right = std::move(child); }

    const KDTree * get_left_child() const { return m_left.get(); }
    const KDTree * get_right_child() const { return m_right.get(); }
    const KDTree * get_parent() const { return m_parent; }

    const NodeType * get_node() const { return m_node; }
    NodeType * get_node() { return m_node; }

    bool is_leaf() const { return m_left == nullptr && m_right == nullptr; }
    bool is_root() const { return m_parent == nullptr; }

protected:
    
private:
    DimType                 m_splitting_dim;
    NodeType *              m_node;

    KDTree *                m_parent;
    std::unique_ptr<KDTree> m_left;
    std::unique_ptr<KDTree> m_right;

}; // class KDTree

template <class TNodeType>
NodeType * kdtree_find_nearest(KDTree<TNodeType> * tree, const TNodeType & query);

} // namespace mh

#include "impl/kd_tree.hpp"

#endif /* KD_TREE_H */
