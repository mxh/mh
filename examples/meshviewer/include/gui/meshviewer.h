#ifndef MESHVIEWER_H
#define MESHVIEWER_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/camera.h"
#include "mh/gui/sceneviewer.h"

namespace mh 
{

class MeshViewer : public SceneViewer
{
public:
    MeshViewer(int width=MH_DEFAULT_VIEWER_WIDTH, int height=MH_DEFAULT_VIEWER_HEIGHT)
        : SceneViewer(width, height)
    { init(); }

    int  init        (void);

    void mainLoop    (void);

protected:

private:
    std::shared_ptr<Camera> m_camera;

}; // class MeshViewer

} // namespace mh

#endif /* MESHVIEWER_H */
