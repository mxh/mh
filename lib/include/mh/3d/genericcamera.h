#ifndef GENERICCAMERA_H
#define GENERICCAMERA_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/3d/camerabase.h"

namespace mh
{

class GenericCamera : public CameraBase
{
public:
    GenericCamera() : m_cameraToClip(Eigen::Matrix4f::Identity()),
                      m_worldToCamera(Eigen::Affine3f::Identity()) {}

    virtual Eigen::Matrix4f getCameraToClip()                { return m_cameraToClip; }
    virtual Eigen::Affine3f getWorldToCamera()               { return m_worldToCamera; }

          Eigen::Matrix4f & getCameraToClipRef  (void)       { return m_cameraToClip;  }
    const Eigen::Matrix4f & getCameraToClipRef  (void) const { return m_cameraToClip;  }

          Eigen::Affine3f & getWorldToCameraRef (void)       { return m_worldToCamera; }
    const Eigen::Affine3f & getWorldToCameraRef (void) const { return m_worldToCamera; }

protected:

private:
    Eigen::Matrix4f m_cameraToClip;
    Eigen::Affine3f m_worldToCamera;

}; // class GenericCamera

} // namespace mh
#endif /* GENERICCAMERA_H */
