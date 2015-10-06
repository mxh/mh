#include "mh/gpu/shader.h"

// Functions from http://www.antongerdelan.net/opengl/shaders.html
// and https://www.opengl.org/wiki/Example_Code
namespace
{

void print_shader_info_log(GLuint shader_index)
{
    GLint max_length = 0;
    glGetShaderiv(shader_index, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> error_log(max_length);
    glGetShaderInfoLog(shader_index, max_length, &max_length, error_log.data());
    std::cout << "Shader " << shader_index << " compile error :" << std::endl;
    for(std::vector<GLchar>::const_iterator i = error_log.begin(); i != error_log.end(); ++i)
        std::cout << *i;
}

void print_program_info_log(GLuint program_index)
{
    GLint max_length = 0;
    glGetProgramiv(program_index, GL_INFO_LOG_LENGTH, &max_length);

    std::vector<GLchar> error_log(max_length);
    glGetProgramInfoLog(program_index, max_length, &max_length, error_log.data());
    std::cout << "Program " << program_index << " linking error :" << std::endl;
    for(std::vector<GLchar>::const_iterator i = error_log.begin(); i != error_log.end(); ++i)
        std::cout << *i;
}

bool compile(GLuint shader_index)
{
    glCompileShader(shader_index);

    // check for compile errors
    int params = -1;
    glGetShaderiv(shader_index, GL_COMPILE_STATUS, &params);
    if (GL_TRUE != params)
    {
        print_shader_info_log(shader_index);
        glDeleteShader(shader_index);
        return false;
    }

    return true;
}

}

namespace mh
{

Shader::~Shader(void)
{
    glDeleteShader  (m_vertexShader);
    glDeleteShader  (m_fragmentShader);
    glDeleteProgram (m_program);
}

bool Shader::vertexShader(const std::string & code)
{
    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char * c_str = code.c_str();
    glShaderSource(m_vertexShader, 1, &c_str, NULL);
    return compile(m_vertexShader);
}

bool Shader::fragmentShader(const std::string & code)
{
    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char * c_str = code.c_str();
    glShaderSource(m_fragmentShader, 1, &c_str, NULL);
    return compile(m_fragmentShader);
}

bool Shader::link(void)
{
    m_program = glCreateProgram();
    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);
    glLinkProgram(m_program);

    GLint isLinked = GL_FALSE;
    glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);
    if (GL_FALSE == isLinked)
    {
        print_program_info_log(m_program);
        glDeleteProgram(m_program);
        return false;
    }

    return true;
}

} // namespace mh
