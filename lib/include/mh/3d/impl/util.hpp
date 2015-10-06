namespace mh
{

template <class _VecType, class _TransformType>
std::vector<_VecType> applyTransform(_TransformType T, const std::vector<_VecType> & data)
{
    std::vector<_VecType> res;
    res.resize(data.size());

    for (size_t i = 0; i < data.size(); ++i)
    {
        _VecType transformed = T * data[i];
        res[i] = transformed;
    }

    return res;
}

} // namespace mh
