#ifndef GPU_UTIL_H
#define GPU_UTIL_H

#define printGLError() \
{ \
    GLenum latestGLerr; \
    while ( ( latestGLerr = glGetError() ) != GL_NO_ERROR) { \
        std::cerr << __FILE__ << ":" << __LINE__ << " " << latestGLerr << std::endl; \
    } \
}

#endif
