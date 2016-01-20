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
      m_texture(std::make_shared<Texture>(filename))
{}

void Background::draw()
{
    m_shader->use();

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->getTextureID());

    glBegin(GL_TRIANGLES);

    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, 1.0f);

    glVertex3f( 1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, 1.0f);

    glEnd();

    glDisable(GL_TEXTURE_2D);
}

} // namespace mh
