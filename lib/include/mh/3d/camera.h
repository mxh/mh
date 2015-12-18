#ifndef CAMERA_H
#define CAMERA_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "mh/ext/imgui/imgui.h"
#include "mh/ext/imgui/imgui_impl_glfw.h"

#include "mh/3d/ray.h"
#include "mh/3d/scene.h"

#include "Eigen/Geometry"

namespace mh
{

struct CameraParameters
{
public:
    // default camera parameters
    CameraParameters()
        : near(0.01f), far(100.0f), fov(45.0f), aspect(16.0 / 9.0),
          position(0.0f, 0.0f, 1.0f), forward(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f) {}

    float near;
    float far;
    float fov;
    float aspect;
    Eigen::Vector3f position;
    Eigen::Vector3f forward;
    Eigen::Vector3f up;
}; // struct CameraParameters

inline std::ostream & operator<< (std::ostream & os, const CameraParameters & cp)
{
    os << cp.near        << "," << cp.far         << "," << cp.fov         << "," << cp.aspect << ","
       << cp.position(0) << "," << cp.position(1) << "," << cp.position(2) << ","
       << cp.forward(0)  << "," << cp.forward(1)  << "," << cp.forward(2)  << ","
       << cp.up(0)       << "," << cp.up(1)       << "," << cp.up(2);

    return os;
}

inline std::istream & operator>> (std::istream & is, CameraParameters & cp)
{
    std::string token;
    std::vector<std::string> tokens;
    for (size_t tokenIdx = 0; tokenIdx < 13 && std::getline(is, token, ','); ++tokenIdx)
    {
        tokens.push_back(token);
    }

    cp.near        = std::stod(tokens[0]);
    cp.far         = std::stod(tokens[1]);
    cp.fov         = std::stod(tokens[2]);
    cp.aspect      = std::stod(tokens[3]);

    cp.position(0) = std::stod(tokens[4]);
    cp.position(1) = std::stod(tokens[5]);
    cp.position(2) = std::stod(tokens[6]);

    cp.forward(0)  = std::stod(tokens[7]);
    cp.forward(1)  = std::stod(tokens[8]);
    cp.forward(2)  = std::stod(tokens[9]);

    cp.up(0)       = std::stod(tokens[10]);
    cp.up(1)       = std::stod(tokens[11]);
    cp.up(2)       = std::stod(tokens[12]);

    return is;
}

class Camera
{
public:
    Camera(float near, float far, float fov, float aspect, float speed, float sensitivity)
        : m_near(near), m_far(far), m_fov(fov), m_aspect(aspect),
          m_speed(speed), m_sensitivity(sensitivity), m_position(0.0f, 0.0f, 1.0f),
          m_forward(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f), m_dirty(true)
    {}

    Camera(const CameraParameters & pm, float speed, float sensitivity)
        : m_near(pm.near), m_far(pm.far), m_fov(pm.fov), m_aspect(pm.aspect),
          m_speed(speed), m_sensitivity(sensitivity), m_position(pm.position),
          m_forward(pm.forward), m_up(pm.up), m_dirty(true)
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

void  updateCameraWithImgui (Camera & camera, const ImGuiIO & io, Eigen::Vector3f center);
void  setCameraLookatScene  (Camera & camera, Scene & scene);
float getIdealSphereDist    (Camera & camera, float radius);
float getIdealSceneDist     (Camera & camera, Scene & scene);

Ray   getRayThroughScreen   (Camera & camera, float x, float y);

void  cameraRotate          (Camera & camera, Eigen::Vector3f center, Eigen::Vector3f axis, float angle);
void  cameraYaw             (Camera & camera, Eigen::Vector3f center, float angle);
void  cameraPitch           (Camera & camera, Eigen::Vector3f center, float angle);
void  cameraRoll            (Camera & camera, Eigen::Vector3f center, float angle);

inline Camera & operator<< (Camera & cam, const CameraParameters & cp)
{
    cam.setNear     (cp.near);
    cam.setFar      (cp.far);
    cam.setFOV      (cp.fov);
    cam.setAspect   (cp.aspect);
    cam.setPosition (cp.position);
    cam.setForward  (cp.forward);
    cam.setUp       (cp.up);

    return cam;
}

inline CameraParameters & operator<< (CameraParameters & cp, const Camera & cam)
{
    cp.near     = cam.getNear();
    cp.far      = cam.getFar();
    cp.fov      = cam.getFOV();
    cp.aspect   = cam.getAspect();
    cp.position = cam.getPosition();
    cp.forward  = cam.getForward();
    cp.up       = cam.getUp();

    return cp;
}


} // namespace mh

#endif /* CAMERA_H */
