#include "mh/3d/mesh.h"

namespace mh
{

void Mesh::updateMinMax() const
{
    if (!m_dirtyBB) return;

    m_min = m_verts[0]->getPosition();
    m_max = m_verts[0]->getPosition();

    for (size_t i = 1; i < nVerts(); ++i)
    {
        m_min(0) = std::min(m_verts[i]->getPosition()(0), m_min(0));
        m_min(1) = std::min(m_verts[i]->getPosition()(1), m_min(1));
        m_min(2) = std::min(m_verts[i]->getPosition()(2), m_min(2));

        m_max(0) = std::max(m_verts[i]->getPosition()(0), m_max(0));
        m_max(1) = std::max(m_verts[i]->getPosition()(1), m_max(1));
        m_max(2) = std::max(m_verts[i]->getPosition()(2), m_max(2));
    }
}

void Mesh::updateGL() const
{
    if (!m_dirtyGL) return;

    m_gl_state.deleteVBO();
    m_gl_state.createVBO();
}

} // namespace mh
