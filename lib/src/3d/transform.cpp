#include "mh/3d/transform.h"

namespace mh
{

Eigen::Affine3f transform_to_mtw(const Transform & transform, bool from_bottom, double mesh_y_size)
{
    if (from_bottom)
    {
        Eigen::Matrix4f scale_translation;
        scale_translation.setIdentity();
        scale_translation(1, 3) = -mesh_y_size / 2;

        Eigen::Matrix4f scale_translation_inv;
        scale_translation_inv.setIdentity();
        scale_translation_inv(1, 3) = transform.getScale()(1) * mesh_y_size / 2;

        Eigen::Matrix4f translation;
        translation.setIdentity();
        translation.block<3, 1>(0, 3) = transform.getPosition();

        Eigen::Matrix4f rotation = Eigen::Matrix4f::Identity();
        rotation.block<3, 3>(0, 0) = transform.getRotation().matrix();

        Eigen::Matrix4f scaling = Eigen::Matrix4f::Identity();
        scaling(0, 0) = transform.getScale()(0);
        scaling(1, 1) = transform.getScale()(1);
        scaling(2, 2) = transform.getScale()(2);

        Eigen::Matrix4f final_transform = translation *
            rotation * scale_translation_inv * scaling
            * scale_translation;
        
        Eigen::Affine3f ret;
        ret.matrix() = final_transform;

        return ret;
    }

    return static_cast<Eigen::Affine3f>(Eigen::Translation3f(transform.getPosition()))
        * transform.getRotation() * Eigen::Scaling(transform.getScale());
}

} // namespace mh
