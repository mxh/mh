#ifndef MESHVIEWER_H
#define MESHVIEWER_H 

#include "base/defs.h"
#include "base/imports.h"

#include "3d/camera.h"
#include "gui/sceneviewer.h"

namespace lp
{

class MeshViewer : public SceneViewer
{
public:
    MeshViewer(int width=LP_DEFAULT_VIEWER_WIDTH, int height=LP_DEFAULT_VIEWER_HEIGHT)
        : SceneViewer(width, height)
    { init(); }

    int  init        (void);

    void mainLoop    (void);

protected:

private:
    std::shared_ptr<Camera> m_camera;

}; // class MeshViewer

} // namespace lp

#endif /* MESHVIEWER_H */
