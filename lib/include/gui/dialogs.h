#ifndef DIALOGS_H
#define DIALOGS_H 

#include "ext/nfd/nfd.h"

namespace lp
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

} // namespace lp

#endif /* DIALOGS_H */
