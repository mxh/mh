#include <algorithm>

#include "mh/3d/camera.h"
#include "mh/3d/mesh.h"
#include "mh/3d/util.h"

namespace
{
    Eigen::Affine3f computeViewMatrix(const Eigen::Vector3f & forward,
                                      const Eigen::Vector3f & up,
                                      const Eigen::Vector3f & position)
    {
        Eigen::Vector3f right = forward.cross(up).normalized();
        Eigen::Vector3f orthogonalUp = right.cross(forward).normalized();

        Eigen::Matrix3f rot;
        rot.col(0) = right;
        rot.col(1) = orthogonalUp;
        rot.col(2) = -forward;

        return Eigen::Translation3f(-rot.transpose() * position) * rot.transpose();
    }

    Eigen::Matrix4f computeProjectionMatrix(float near, float far, float fov, float aspect)
    {
        float range = std::tan(fov * 0.5f * M_PI / 180.0f) * near;

        float Sx = (2.0f * near) / (range * aspect + range * aspect);
        float Sy = near / range;
        float Sz = -(far + near) / (far - near);

        float Pz = -(2.0f * far * near) / (far - near);

        Eigen::Matrix4f proj;
        proj << Sx,   0.0f, 0.0f,  0.0f,
                0.0f, Sy,   0.0f,  0.0f,
                0.0f, 0.0f, Sz,    Pz,
                0.0f, 0.0f, -1.0f, 0.0f;

        return proj;
    }
}

namespace mh
{

Eigen::Matrix4f Camera::getCameraToClip(void)
{
    if (m_dirty)
    {
        recomputeTransforms();
    }

    return m_cameraToClip;
}

Eigen::Affine3f Camera::getWorldToCamera(void)
{
    if (m_dirty)
    {
        recomputeTransforms();
    }

    return m_worldToCamera;
}

void Camera::recomputeTransforms(void)
{
    m_cameraToClip  = computeProjectionMatrix(m_near, m_far, m_fov, m_aspect);
    m_worldToCamera = computeViewMatrix(m_forward, m_up, m_position);
}

void updateCameraWithImgui(Camera & camera, const ImGuiIO & io, Eigen::Vector3f center)
{
    Eigen::Vector3f position_delta(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f right;
    right = camera.getForward().cross(camera.getUp()).normalized();

    if (io.KeysDown[io.KeyMap[ImGuiKey_W]])
    {
        position_delta += camera.getForward() * camera.getSpeed();
    }
    if (io.KeysDown[io.KeyMap[ImGuiKey_S]])
    {
        position_delta -= camera.getForward() * camera.getSpeed();
    }
    if (io.KeysDown[io.KeyMap[ImGuiKey_D]])
    {
        position_delta += right * camera.getSpeed();
    }
    if (io.KeysDown[io.KeyMap[ImGuiKey_A]])
    {
        position_delta -= right * camera.getSpeed();
    }
    if (io.KeysDown[io.KeyMap[ImGuiKey_R]])
    {
        position_delta += camera.getUp() * camera.getSpeed();
    }
    if (io.KeysDown[io.KeyMap[ImGuiKey_F]])
    {
        position_delta -= camera.getUp() * camera.getSpeed();
    }

    camera.setPosition(camera.getPosition() + position_delta);

    if (io.MouseWheel != 0)
    {
        camera.setFOV(camera.getFOV() + (io.MouseWheel * MH_DEFAULT_ZOOM_SPEED));
    }

    if (!(io.MouseDown[0] || io.MouseDown[2]) || ImGui::IsMouseHoveringAnyWindow())
    {
        return;
    }

    if (io.MouseDown[0])
    {
        ImVec2 mouseDelta = io.MouseDelta;
        float yawAngle   = -mouseDelta.x * camera.getSensitivity();
        float pitchAngle = -mouseDelta.y * camera.getSensitivity();

        Eigen::Quaternionf rotation = Eigen::AngleAxisf(pitchAngle, camera.getForward().cross(camera.getUp()).normalized()) *
                                      Eigen::AngleAxisf(yawAngle, camera.getUp());

        Eigen::Vector3f newCameraPos = Eigen::Translation3f(center) * rotation * Eigen::Translation3f(-center) * camera.getPosition();

        camera.setForward(rotation * camera.getForward());
        camera.setUp(camera.getForward().cross(camera.getUp()).normalized().cross(camera.getForward()).normalized());
        camera.setPosition(newCameraPos);

    } else if (io.MouseDown[2])
    {
        ImVec2 mouseDelta = io.MouseDelta;
        Eigen::Vector3f translation = camera.getUp() * mouseDelta.y + camera.getForward().cross(camera.getUp()).normalized() * -mouseDelta.x;
        Eigen::Vector3f newCameraPos = Eigen::Translation3f(translation) * camera.getPosition();
        camera.setPosition(newCameraPos);
    }
}

void setCameraLookatScene(Camera & camera, Scene & scene)
{
    Eigen::Vector3f center = scene.getCenter();

    std::vector<float> radii;
    radii.resize(scene.getMeshes().size());
    std::transform(scene.getMeshes().begin(), scene.getMeshes().end(), radii.begin(),
        [&center, &camera] (const std::shared_ptr<Mesh> & x)
        {
            return getFittingSphereRadius(applyTransform(x->getModelToWorld(), x->getVertData()), center);
        }
    );
    
    float r = *std::max_element(radii.begin(), radii.end());
    float dist = r / std::sin(0.5 * camera.getFOV() * M_PI / 180);

    camera.setPosition(center - camera.getForward() * dist);
    camera.setNear(dist - r);
    camera.setFar(dist + r);
}

} // namespace mh
