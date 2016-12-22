#include "mh/util/background.h"

#include "mh/gpu/shader.h"

namespace
{
    using namespace mh;

    inline std::shared_ptr<Shader> compileBackgroundShader()
    {
        auto shader = std::make_shared<Shader>();

        shader->vertexShader(
        GLSL(330,
             const vec2 madd = vec2(0.5, 0.5);
             in vec2 vertexIn;
             out vec2 textureCoord;

             void main()
             {
                textureCoord = vertexIn.xy * madd + madd;
                gl_Position = vec4(vertexIn.xy, 0.0, 1.0);
             }
         ));

        shader->fragmentShader(
        GLSL(330,
             uniform sampler2D tex;

             in vec2 textureCoord;

             out vec4 fragColor;

             void main()
             {
                 fragColor = texture2D(tex, textureCoord);
                 //fragColor = vec4(clampedCosine * color, 1.0);
             }
         ));

        shader->link();

        return shader;
    }   

} // anonymous namespace

namespace mh
{

Background::Background(const std::string & filename)
    : m_shader(compileBackgroundShader()),
      m_texture(std::make_shared<Texture>(filename)),
      m_filename(filename)
{
    static float triangles [] = {-1.0f, -1.0f, 1.0f,
                                 -1.0f,  1.0f, 1.0f,
                                  1.0f, -1.0f, 1.0f,

                                  1.0f, -1.0f, 1.0f,
                                 -1.0f,  1.0f, 1.0f,
                                  1.0f,  1.0f, 1.0f};  

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, triangles, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void Background::draw()
{
    m_shader->use();

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);
}

} // namespace mh
