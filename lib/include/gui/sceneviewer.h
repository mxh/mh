#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H 

#include "base/defs.h"
#include "base/imports.h"

#include "3d/scene.h"
#include "gui/viewer.h"

namespace lp
{

class SceneViewer : public Viewer
{
public:
    SceneViewer(int width=LP_DEFAULT_VIEWER_WIDTH, int height=LP_DEFAULT_VIEWER_HEIGHT)
        : Viewer(width, height), m_scene(std::make_shared<Scene>()) {}

    EXPOSE_SHARED_PTR(Scene, Scene, m_scene)

protected:
    std::shared_ptr<Scene> m_scene;

private:

}; // class SceneViewer

} // namespace lp

#endif /* SCENEVIEWER_H */
