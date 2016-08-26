#ifndef DIALOGS_H
#define DIALOGS_H 

#include "mh/ext/nfd/nfd.h"

namespace mh
{

inline std::string openFileDialog(const std::string & filters)
{
    nfdchar_t   *cpath = NULL;
    nfdresult_t result = NFD_OpenDialog(filters.c_str(), NULL, &cpath);

    if (result != NFD_OKAY)
    {
        return std::string();
    }

    std::string path(cpath);
    free(cpath);

    return path;
}

inline std::string saveFileDialog(const std::string & filters)
{
    nfdchar_t   *cpath = NULL;
    nfdresult_t result = NFD_SaveDialog(filters.c_str(), NULL, &cpath);

    if (result != NFD_OKAY)
    {
        return std::string();
    }

    std::string path(cpath);
    free(cpath);

    return path;
}

} // namespace mh

#endif /* DIALOGS_H */
