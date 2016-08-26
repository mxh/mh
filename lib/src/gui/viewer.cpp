#include "mh/gui/viewer.h"

#include <iostream>

#include "mh/gpu/gpu_util.h"

#include "mh/ext/gl3w/gl3w.h"

namespace mh
{

int Viewer::baseInit(void)
{
    //printGLError();
    //// GLFW setup
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit())
    {
        std::cerr << "Could not initialize GLFW." << std::endl;
        return 1;
    }
    printGLError();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
    printGLError();

    glfwMakeContextCurrent(m_window);
    printGLError();
    gl3wInit();

    glfwSwapInterval(1);
    printGLError();

    /*glewExperimental = GL_TRUE;

    //// GLEW setup
    GLenum glewError = glewInit();
    if (GLEW_OK != glewError)
    {
        std::cerr << "Could not initialize GLEW." << std::endl;
        return 1;
    }*/

    //// Imgui setup
    /*m_imguiState = (void *) malloc(ImGui::GetInternalStateSize());
    ImGui::SetInternalState(m_imguiState, true);*/
    ImGui_ImplGlfwGL3_Init(m_window, true);
    printGLError();

    return 0;
}

void Viewer::makeCurrent(void)
{
    glfwMakeContextCurrent(m_window);
    /*ImGui::SetInternalState(m_imguiState, false);
    ImGui_ImplGlfwGL3_Init(m_window, true);
    ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
    ImGui_ImplGlfwGL3_CreateDeviceObjects();*/
}

void Viewer::pollSize(void)
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
}

} // namespace mh
