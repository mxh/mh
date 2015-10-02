#ifndef DEFS_H
#define DEFS_H 

#include <cstdlib>
#include <iostream>

namespace lp
{

#ifdef DEBUG
#   define LP_DEBUG 1
#else
#   define LP_DEBUG 0
#endif

#if (LP_DEBUG)
#   define LP_ASSERT(X) ((void)(!(X) && lp::assertFailure(#X, __FILE__, __LINE__) && (HALT(), 1)))
#else
#   define LP_ASSERT(X) ((void)(0))
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

#define LP_DEFAULT_VIEWER_WIDTH  1280
#define LP_DEFAULT_VIEWER_HEIGHT 720
#define LP_DEFAULT_VIEWER_TITLE  "Untitled viewer"

#define LP_DEFAULT_WIDGET_WIDTH  300
#define LP_DEFAULT_WIDGET_HEIGHT 300
#define LP_DEFAULT_WIDGET_TITLE  "Untitled widget"

} // namespace lp

#endif /* DEFS_H */
