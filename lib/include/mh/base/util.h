#ifndef MH_UTIL_H
#define MH_UTIL_H 

#include <memory>
#include <vector>

namespace mh
{

template <class T>
std::shared_ptr<T> clone_shared_ptr(std::shared_ptr<T> obj)
{
    return obj.get() != nullptr ? std::make_shared<T>(*obj) : nullptr;
}

template <class T>
std::unique_ptr<T> clone_unique_ptr(std::unique_ptr<T> obj)
{
    return obj.get() != nullptr ? std::make_unique<T>(*obj) : nullptr;
}

template <class T>
std::vector<std::shared_ptr<T> > clone_vec_shared_ptr(const std::vector<std::shared_ptr<T> > & vec)
{
    std::vector<std::shared_ptr<T> > out_vec;
    for (auto ptr : vec)
    {
        out_vec.push_back(clone_shared_ptr(ptr))
    }

    return out_vec;
}

template <class T>
std::vector<std::unique_ptr<T> > clone_vec_unique_ptr(const std::vector<std::unique_ptr<T> > & vec)
{
    std::vector<std::unique_ptr<T> > out_vec;
    for (auto ptr : vec)
    {
        out_vec.push_back(clone_unique_ptr(ptr))
    }

    return out_vec;
}

} // namespace mh

#endif /* MH_UTIL_H */
