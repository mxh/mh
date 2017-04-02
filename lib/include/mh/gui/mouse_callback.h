#ifndef MOUSE_CALLBACK_H
#define MOUSE_CALLBACK_H

#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include "mh/ext/gl3w/gl3w.h"
#include <GLFW/glfw3.h>

namespace mh
{

class MouseCallback
{
public:
    virtual ~MouseCallback() {}

    void clickUpdate(int button)
    { m_dragging = true; m_draggingButton = button; clickUpdateImpl(button); }
    void dragUpdate(float x, float y)
    { m_mouseX = x; m_mouseY = y; m_dragging ? dragUpdateImpl() : moveUpdateImpl(); }
    void releaseUpdate(int button)
    { m_dragging = false; releaseUpdateImpl(button); }
    
    // called when a click happens
    virtual void clickUpdateImpl(int button)   {}
    // called when a drag happens
    virtual void dragUpdateImpl()              {}
    // called when a move happens
    virtual void moveUpdateImpl()              {}
    // called when the mouse is released
    virtual void releaseUpdateImpl(int button) {}

    virtual const char * getName() const = 0;

    virtual const char * getUIName() const = 0;

protected:
    bool           m_dragging;
    int            m_draggingButton;

    float          m_mouseX;
    float          m_mouseY;

}; // class MouseCallback

class MouseCallbackConfig
{
public:
    MouseCallbackConfig() : m_activeCallback(-1) {}

    void addMouseCallback(std::unique_ptr<MouseCallback> callback)
    { m_callbacks.push_back(std::move(callback));
      m_stringToIdx[m_callbacks.back()->getName()] = m_callbacks.size() - 1;
    }

    void setActiveCallback(const std::string & name) { m_activeCallback = m_stringToIdx[name]; }
    void deactivateCallback() { m_activeCallback = -1; }
    bool isActive() { return m_activeCallback != -1; }

    const std::vector<std::unique_ptr<MouseCallback> > & getCallbacks() const
    { return m_callbacks; }

          MouseCallback & getActiveCallback()       { return *m_callbacks[m_activeCallback]; }
    const MouseCallback & getActiveCallback() const { return *m_callbacks[m_activeCallback]; }

protected:
    std::vector<std::unique_ptr<MouseCallback> > m_callbacks;
    std::map<std::string, size_t>                m_stringToIdx;

    int                                          m_activeCallback;

}; // class MouseCallbackConfig

class GLFWMouseCallbackManager
{
public:
    static GLFWMouseCallbackManager & getInstance()
    {
        static GLFWMouseCallbackManager instance;
        return instance;
    }

    void setMouseCallbackConfig(MouseCallbackConfig * config)
    { m_mouseCallbackConfig = config; }

    static void mouseButtonCallback(GLFWwindow * window, int key, int action, int mods)
    {
        auto & manager = getInstance();
        if (manager.m_mouseCallbackConfig && manager.m_mouseCallbackConfig->isActive())
        {
            if (action == GLFW_PRESS)
            {
                manager.m_mouseCallbackConfig->getActiveCallback().clickUpdate(key);
            } else {
                manager.m_mouseCallbackConfig->getActiveCallback().releaseUpdate(key);
            }
        }
    }

    static void mouseMoveCallback(GLFWwindow * window, double x, double y)
    {
        auto & manager = getInstance();
        if (manager.m_mouseCallbackConfig && manager.m_mouseCallbackConfig->isActive())
        {
            manager.m_mouseCallbackConfig->getActiveCallback().dragUpdate(x, y); 
        }
    }

private:
    GLFWMouseCallbackManager() {}
    GLFWMouseCallbackManager(const GLFWMouseCallbackManager &);
    void operator=(const GLFWMouseCallbackManager &);

    MouseCallbackConfig * m_mouseCallbackConfig;

}; // class MouseCallbackManager

} // namespace mh 

#endif /* MOUSE_CALLBACK_H */
