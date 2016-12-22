namespace mh
{

template <class TNodeType>
NodeType * kdtree_find_nearest(KDTree<TNodeType> * tree, const TNodeType & query)
{
    typedef KDTreeTraits<TNodeType> Traits;

    while (!tree.is_leaf())
    {
        if (Traits::compare(query, *tree->get_node()))
        {
            tree = tree->get_left_child();
        } else {
            tree = tree->get_right_child();
        }
    }
    TNodeType * best_node = tree->get_node();
    Traits::DiffType best_diff = Traits::diff(query, *best_node);

    while (!tree.is_root()) 
    {
        tree = tree->get_parent();
        // check whether a better node could be at the other side of the splitting plane
        
        // check if the parent node is better

        TNodeType * node = tree->get_node();
        Traits::DiffType diff = Traits::diff(query, *node);

        if (diff < best_diff)
        {
            best_node = node;
            best_diff = diff;
        }

        // check if a node at the other side of the splitting plane might be better
        Traits::intersect_split(query, best_diff, tree
    }
}

}
