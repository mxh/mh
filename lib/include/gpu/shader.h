#ifndef SHADER_H
#define SHADER_H 

#include "base/defs.h"
#include "base/imports.h"

#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "eigen3/Eigen/Geometry"

#define GLSL(version, shader)  std::string("#version " #version "\n" #shader)

#define EXPOSE_UNIFORM_SETTER_MATRIX(TYPE, EXT) \
void setUniform(const std::string & name, TYPE value) \
{ \
    int location = glGetUniformLocation(m_program, name.c_str()); \
    glUniformMatrix ## EXT(location, 1, GL_FALSE, value.data()); \
}

#define EXPOSE_UNIFORM_SETTER_BUILTIN(TYPE, EXT) \
void setUniform(const std::string & name, TYPE value) \
{ \
    int location = glGetUniformLocation(m_program, name.c_str()); \
    glUniform1 ## EXT(location, value); \
}

namespace lp
{

class Shader
{
public:
    Shader  (void) {}
    Shader  (const std::string & vertexCode, const std::string & fragmentCode)
    { vertexShader(vertexCode); fragmentShader(fragmentCode); }
    ~Shader (void);
    
    EXPOSE_UNIFORM_SETTER_MATRIX  (const Eigen::Matrix2f,  2fv);
    EXPOSE_UNIFORM_SETTER_MATRIX  (const Eigen::Matrix3f&, 3fv);
    EXPOSE_UNIFORM_SETTER_MATRIX  (const Eigen::Matrix4f&, 4fv);
    EXPOSE_UNIFORM_SETTER_MATRIX  (const Eigen::Affine3f&, 4fv);

    EXPOSE_UNIFORM_SETTER_BUILTIN (float, f);
    EXPOSE_UNIFORM_SETTER_BUILTIN (int, i);

    bool vertexShader   (const std::string & code);
    bool fragmentShader (const std::string & code);

    bool link           (void);
    void use            (void) { glUseProgram(m_program); }

protected:

private:
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_program;

}; // class Shader

} // namespace lp

#endif /* SHADER_H */
