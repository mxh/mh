#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include "Eigen/Geometry"

namespace mh
{

class Transform
{
public:
    Transform() : m_position(0.0f, 0.0f, 0.0f),
                  m_scale(1.0f),
                  m_rotation(Eigen::AngleAxisf(0.0f, Eigen::Vector3f::UnitX())) {}
    void              setPosition(Eigen::Vector3f position)         { m_position = position; }
    Eigen::Vector3f   getPosition()                           const { return m_position; }

    void              setScale(float scale)                         { m_scale = scale; }
    float             getScale()                              const { return m_scale;}

    void              setRotation(Eigen::AngleAxisf rotation)       { m_rotation = rotation;}
    Eigen::AngleAxisf getRotation()                           const { return m_rotation; }
    
protected:

private:
    Eigen::Vector3f   m_position;
    float             m_scale;
    Eigen::AngleAxisf m_rotation;

}; // class Transform

Eigen::Affine3f transform_to_mtw(const Transform & transform);

} // namespace mh
