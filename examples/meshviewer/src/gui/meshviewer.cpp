#include "gui/meshviewer.h"

#include "3d/camera.h"

#include "gui/dialogs.h"

#include "io/meshio.h"

#include "gpu/shader.h"

namespace
{

using namespace lp;

std::shared_ptr<Shader> compileDefaultShader()
{
    auto shader = std::make_shared<Shader>();
    
    shader->vertexShader(
    GLSL(330,
         layout(location = 0) in vec3 vertexPosition;
         layout(location = 1) in vec3 vertexNormal;
         layout(location = 2) in vec3 vertexColor;

         uniform mat4 cameraToClip;
         uniform mat4 worldToCamera;
         uniform mat4 modelToWorld;

         out vec3 position;
         out vec3 normal;
         out vec3 color;
         
         void main()
         {
             position = (worldToCamera * modelToWorld * vec4(vertexPosition, 1.0)).xyz;
             normal   = (worldToCamera * modelToWorld * vec4(vertexNormal, 0.0)).xyz;
             color    = vertexColor;

             gl_Position = cameraToClip * vec4(position, 1.0);
         }
     ));

    shader->fragmentShader(
    GLSL(330,
         in vec3 position;
         in vec3 normal;
         in vec3 color;

         out vec4 fragColor;

         void main()
         {
             vec3 pixelNormal = normalize(normal);
             vec3 viewDirection = normalize(-position);
             float clampedCosine = max(dot(pixelNormal, viewDirection), 0.0);
             fragColor = vec4(clampedCosine * color, 1.0);
             fragColor.a = 0.5;
         }
     ));

    shader->link();

    return shader;
}

}

namespace lp
{

int MeshViewer::init(void)
{
    //// OpenGL setup
    glEnable    (GL_DEPTH_TEST);
    glDepthFunc (GL_LESS);

    glDisable   (GL_CULL_FACE);
    glCullFace  (GL_BACK);
    glFrontFace (GL_CCW);

    glEnable    (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize (5.0);

    //// Default camera setup
    float near = 0.01f;
    float far  = 100.0f;
    float fov  = 45.0f;
    float speed = 0.01f;
    float sensitivity = 1.0f / 1000;
    
    m_camera = std::make_shared<Camera>(near, far, fov, 16.0 / 9.0, speed, sensitivity);

    //// Default shader setup
    addShader("default", compileDefaultShader());

    return 0;
}

void MeshViewer::mainLoop()
{
    glfwPollEvents();

    ImGui_ImplGlfw_NewFrame();

    pollSize();
    m_camera->setAspect((float) m_width / (float) m_height);

    ImGuiIO& io = ImGui::GetIO();
    if (io.KeysDown[io.KeyMap[ImGuiKey_Escape]])
    {
        setShouldClose(true);
    }

    if (m_scene)
    {
        updateCameraWithImgui(*m_camera, io, m_scene->getCenter());
    }

    if (ImGui::Button("Load mesh"))
    {
        std::string filename = openFileDialog("obj");
        if (filename != "")
        {
            std::vector<std::shared_ptr<Mesh> > meshes = loadMeshesFromOBJ(filename);

            m_scene->addMeshes(meshes);
            setCameraLookatScene(*m_camera, *m_scene);            
        }
    }

    //// Draw calls
    glViewport(0, 0, m_width, m_height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLineWidth(2.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe

    if (m_scene)
    {
        m_scene->draw(m_shaders["default"], m_camera);
    }

    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    ImGui::Render();
}

}
