#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "Eigen/Geometry"

namespace mh
{

class Transform
{
public:
    Transform() : m_position(0.0f, 0.0f, 0.0f),
                  m_scale({1.0f, 1.0f, 1.0f}),
                  m_rotation(Eigen::AngleAxisf(0.0f, Eigen::Vector3f::UnitX())) {}
    void              setPosition(Eigen::Vector3f position)         { m_position = position; }
    Eigen::Vector3f   getPosition()                           const { return m_position; }

    void              setScale(float scale)                         { m_scale = {scale, scale, scale}; }
    void              setScale(const Eigen::Vector3f & scale)       { m_scale = scale; }
    Eigen::Vector3f   getScale()                              const { return m_scale;}

    void              setRotation(Eigen::AngleAxisf rotation)       { m_rotation = rotation;}
    Eigen::AngleAxisf getRotation()                           const { return m_rotation; }
    
protected:

private:
    Eigen::Vector3f   m_position;
    Eigen::Vector3f   m_scale;
    Eigen::AngleAxisf m_rotation;

}; // class Transform

Eigen::Affine3f transform_to_mtw(const Transform & transform, bool from_bottom=false, double mesh_y_size=0);
Eigen::Affine3f mtw_to_transform(const Transform & transform);

} // namespace mh
