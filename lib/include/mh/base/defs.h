#ifndef DEFS_H
#define DEFS_H 

#include <cstdlib>
#include <iostream>

namespace mh
{

#ifdef DEBUG
#   define MH_DEBUG 1
#else
#   define MH_DEBUG 0
#endif

#if (MH_DEBUG)
#   define MH_ASSERT(X) ((void)(!(X) && mh::assertFailure(#X, __FILE__, __LINE__) && (HALT(), 1)))
#else
#   define MH_ASSERT(X) ((void)(0))
#endif

#define HALT() exit(EXIT_FAILURE)

inline bool assertFailure(const char * msg, const char * file, int line)
{
    std::cerr << "Assertion failure! " << file << ":" << line << std::endl;
    std::cerr << msg << std::endl;

    return true;
}

#define EXPOSE(TYPE, NAME, VARNAME) \
    const TYPE & get##NAME(void) const { return VARNAME; } \
    void set##NAME(const TYPE & val) { VARNAME = val; }

#define EXPOSE_DIRTY(TYPE, NAME, VARNAME) \
    const TYPE & get##NAME(void) const { return VARNAME; } \
    void set##NAME(const TYPE & val) { VARNAME = val; m_dirty = true; }

#define EXPOSE_SHARED_PTR(TYPE, NAME, VARNAME) \
    std::shared_ptr<const TYPE> get##NAME(void) const { return VARNAME; } \
    std::shared_ptr<TYPE> get##NAME(void) { return VARNAME; } \
    void set##NAME(std::shared_ptr<TYPE> val) { VARNAME = val; }

#define EXPOSE_PTR(TYPE, NAME, VARNAME) \
    const TYPE * get##NAME(void) const { return VARNAME; } \
          TYPE * get##NAME(void)       { return VARNAME; } \
    void set##NAME(TYPE * val) { VARNAME = val; }

#define MH_DEFAULT_VIEWER_WIDTH  1280
#define MH_DEFAULT_VIEWER_HEIGHT 720
#define MH_DEFAULT_VIEWER_TITLE  "Untitled viewer"

#define MH_DEFAULT_WIDGET_WIDTH  300
#define MH_DEFAULT_WIDGET_HEIGHT 300
#define MH_DEFAULT_WIDGET_TITLE  "Untitled widget"

#define MH_DEFAULT_ZOOM_SPEED    10

#define MH_EPSILON               1e-5

} // namespace mh

#endif /* DEFS_H */
