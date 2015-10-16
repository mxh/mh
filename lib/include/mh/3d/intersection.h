#ifndef INTERSECTION_H
#define INTERSECTION_H 

namespace mh
{

template <class TObjectType>
class Intersection
{
public:
    typedef TObjectType ObjectType;

    Intersection() 
        : m_hit(false), m_t(std::numeric_limits<float>::max()), m_object(nullptr) {}
    Intersection(bool hit, float t, ObjectType * object)
        : m_hit(hit), m_t(t), m_object(object) {}

    operator     bool()      const { return m_hit; }

    bool         getHit()    const { return m_hit; }
    float        getT()      const { return m_t; }
    ObjectType * getObject()       { return m_object; }
    
protected:

private:
    bool         m_hit;
    float        m_t;
    ObjectType * m_object;

}; // class Intersection

} // namespace mh

#endif /* INTERSECTION_H */
