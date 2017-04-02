#include "mh/gui/viewer.h"

#include <iostream>

#include "mh/gpu/gpu_util.h"

#include "mh/ext/gl3w/gl3w.h"

namespace mh
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);

    glfwMakeContextCurrent(m_window);
    gl3wInit();

    glfwSwapInterval(1);
    printGLError();

    ImGui_ImplGlfwGL3_Init(m_window, true);
    printGLError();

    glfwSetMouseButtonCallback(m_window, &GLFWMouseCallbackManager::mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, &GLFWMouseCallbackManager::mouseMoveCallback);
    GLFWMouseCallbackManager::getInstance().setMouseCallbackConfig(&m_mouseCallbackConfig);

    return 0;
}

void Viewer::makeCurrent(void)
{
    glfwMakeContextCurrent(m_window);
}

void Viewer::pollSize(void)
{
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
}

} // namespace mh
