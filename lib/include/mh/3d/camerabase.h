#ifndef CAMERABASE_H
#define CAMERABASE_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "Eigen/Core"
#include "Eigen/Geometry"

namespace mh
{

class CameraBase
{
public:
    virtual Eigen::Matrix4f getCameraToClip(void) = 0;
    virtual Eigen::Affine3f getWorldToCamera(void) = 0;

protected:

private:

}; // class CameraBase

} // namespace mh

#endif /* CAMERABASE_H */
