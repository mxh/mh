#ifndef RAY_H
#define RAY_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

namespace mh
{

class Ray
{
public:
    Ray() {} 

    Ray(Eigen::Vector3f position, Eigen::Vector3f direction)
        : m_position(position), m_direction(direction.normalized()) {}

    EXPOSE(Eigen::Vector3f, Position, m_position);

    Eigen::Vector3f getDirection (void) const               { return m_direction; }
    void            setDirection (Eigen::Vector3f direction) { m_direction = direction.normalized(); }

protected:

private:
    Eigen::Vector3f m_position;
    Eigen::Vector3f m_direction;

}; // class Ray

} // namespace mh

#endif /* RAY_H */
