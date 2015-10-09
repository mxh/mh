#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/scene.h"
#include "mh/gui/viewer.h"

namespace mh
{

class SceneViewer : public Viewer
{
public:
    SceneViewer(int width=MH_DEFAULT_VIEWER_WIDTH, int height=MH_DEFAULT_VIEWER_HEIGHT)
        : Viewer(width, height), m_scene(std::make_shared<Scene>()) {}

    EXPOSE_SHARED_PTR(Scene, Scene, m_scene)

protected:
    std::shared_ptr<Scene> m_scene;

private:

}; // class SceneViewer

} // namespace mh

#endif /* SCENEVIEWER_H */
