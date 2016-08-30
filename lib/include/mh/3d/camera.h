#ifndef CAMERA_H
#define CAMERA_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/ext/imgui/imgui.h"
#include "mh/ext/imgui/imgui_impl_glfw.h"

#include "mh/3d/scene.h"

#include "Eigen/Geometry"

namespace mh
{

class Camera
{
public:
    Camera(float near, float far, float fov, float aspect, float speed, float sensitivity)
        : m_near(near), m_far(far), m_fov(fov), m_aspect(aspect),
          m_speed(speed), m_sensitivity(sensitivity), m_position(0.0f, 0.0f, 1.0f),
          m_forward(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f), m_dirty(true)
    {}

    EXPOSE_DIRTY(float,           Near,        m_near);
    EXPOSE_DIRTY(float,           Far,         m_far);
    EXPOSE_DIRTY(float,           FOV,         m_fov);
    EXPOSE_DIRTY(float,           Aspect,      m_aspect);
    EXPOSE_DIRTY(float,           Speed,       m_speed);
    EXPOSE_DIRTY(float,           Sensitivity, m_sensitivity);
    EXPOSE_DIRTY(Eigen::Vector3f, Position,    m_position);
    EXPOSE_DIRTY(Eigen::Vector3f, Forward,     m_forward);
    EXPOSE_DIRTY(Eigen::Vector3f, Up,          m_up);

    Eigen::Matrix4f getCameraToClip    (void);
    Eigen::Affine3f getWorldToCamera   (void);

    void setCameraRotation(const Eigen::Matrix3f & M);

protected:
    void            recomputeTransforms(void);

private:
    float           m_near;
    float           m_far;
    float           m_fov;
    float           m_aspect;
    float           m_speed;
    float           m_sensitivity;

    Eigen::Matrix4f m_cameraToClip;
    Eigen::Affine3f m_worldToCamera;

    Eigen::Vector3f m_position;
    Eigen::Vector3f m_forward;
    Eigen::Vector3f m_up;

    bool            m_dirty;

}; // class Camera

void updateCameraWithImgui(Camera & camera, const ImGuiIO & io, Eigen::Vector3f center);
//void setCameraLookatScene(Camera & camera, Scene & scene);

void cameraRotate(Camera & camera, Eigen::Vector3f center, Eigen::Vector3f axis, float angle);
void cameraYaw   (Camera & camera, Eigen::Vector3f center, float angle);
void cameraPitch (Camera & camera, Eigen::Vector3f center, float angle);
void cameraRoll  (Camera & camera, Eigen::Vector3f center, float angle);


} // namespace mh
#endif /* CAMERA_H */
