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

inline std::vector<std::string> openFileDialogMultiple(const std::string & filters)
{
    nfdpathset_t pathset;
    nfdresult_t  result = NFD_OpenDialogMultiple(filters.c_str(), NULL, &pathset);

    std::vector<std::string> paths;

    if (result != NFD_OKAY)
    {
        return std::vector<std::string>();
    }

    for (size_t i = 0; i < NFD_PathSet_GetCount(&pathset); ++i)
    {
        nfdchar_t *path = NFD_PathSet_GetPath(&pathset, i);
        paths.push_back(std::string(path));
    }

    NFD_PathSet_Free(&pathset);
}

} // namespace mh

#endif /* DIALOGS_H */
