namespace mh
{

template <class TVecType, class TTransformType>
std::vector<TVecType> applyTransform(TTransformType T, const std::vector<TVecType> & data)
{
    std::vector<TVecType> res;
    res.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        TVecType transformed = T * data[i];
        res[i] = transformed;
    }

    return res;
}

} // namespace mh
