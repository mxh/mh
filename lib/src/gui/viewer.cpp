#include "gui/viewer.h"

namespace lp
{

int Viewer::baseInit(void)
{
    //// GLFW setup
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW." << std::endl;
        return 1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    glfwMakeContextCurrent(m_window);

    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;

    //// GLEW setup
    GLenum glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        std::cerr << "Could not initialize GLEW." << std::endl;
        return 1;
    }

    //// Imgui setup
    m_imguiState = (void *) malloc(ImGui::GetInternalStateSize());
    ImGui::SetInternalState(m_imguiState, true);
    ImGui_ImplGlfw_Init(m_window, true);

    return 0;
}

void Viewer::makeCurrent(void)
{
    glfwMakeContextCurrent(m_window);
    ImGui::SetInternalState(m_imguiState, false);
    ImGui_ImplGlfw_Init(m_window, true);
    ImGui_ImplGlfw_InvalidateDeviceObjects();
    ImGui_ImplGlfw_CreateDeviceObjects();
}

void Viewer::pollSize(void)
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
}

} // namespace lp
