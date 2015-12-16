#ifndef DATATYPES_H
#define DATATYPES_H 

#include <cstdint>

#include "Eigen/Core"

namespace mh
{

struct int2 {
    int32_t x;
    int32_t y;

    const int32_t & operator[](int i) const { return ((int32_t*)this)[i]; }
          int32_t & operator[](int i)       { return ((int32_t*)this)[i]; }
}; // struct int2

struct int3 {
    int32_t x;
    int32_t y;
    int32_t z;

    const int32_t & operator[](int i) const { return ((int32_t*)this)[i]; }
          int32_t & operator[](int i)       { return ((int32_t*)this)[i]; }
}; // struct int3

struct int4 {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;

    const int32_t & operator[](int i) const { return ((int32_t*)this)[i]; }
          int32_t & operator[](int i)       { return ((int32_t*)this)[i]; }
}; // struct int4

inline int2 makeInt2(int32_t x, int32_t y)
{
    int2 i;
    i.x = x;
    i.y = y;
    return i;
}
inline int2 makeInt2(Eigen::Vector2i i)     { return makeInt2(i(0), i(1));      }
inline Eigen::Vector2i makeEigVec2i(int2 i) { return Eigen::Vector2i(i.x, i.y); }

inline int3 makeInt3(int32_t x, int32_t y, int32_t z)
{
    int3 i;
    i.x = x;
    i.y = y;
    i.z = z;
    return i;
}
inline int3 makeInt3(Eigen::Vector3i i)     { return makeInt3(i(0), i(1), i(2));     }
inline Eigen::Vector3i makeEigVec3i(int3 i) { return Eigen::Vector3i(i.x, i.y, i.z); }

inline int4 makeInt4(int32_t x, int32_t y, int32_t z, int32_t w)
{
    int4 i;
    i.x = x;
    i.y = y;
    i.z = z;
    i.w = w;
    return i;
}
inline int4 makeInt4(Eigen::Vector4i i)     { return makeInt4(i(0), i(1), i(2), i(3));    }
inline Eigen::Vector4i makeEigVec4i(int4 i) { return Eigen::Vector4i(i.x, i.y, i.z, i.w); }

struct float2 {
    float x;
    float y;

    const float & operator[](int i) const { return ((float*)this)[i]; }
          float & operator[](int i)       { return ((float*)this)[i]; }
}; // struct float2

struct float3 {
    float x;
    float y;
    float z;

    const float & operator[](int i) const { return ((float*)this)[i]; }
          float & operator[](int i)       { return ((float*)this)[i]; }
}; // struct float3

struct float4 {
    float x;
    float y;
    float z;
    float w;

    const float & operator[](int i) const { return ((float*)this)[i]; }
          float & operator[](int i)       { return ((float*)this)[i]; }
}; // struct float4

inline float2 makeFloat2(float x, float y)
{
    float2 f;
    f.x = x;
    f.y = y;
    return f;
}
inline float2 makeFloat2(Eigen::Vector2f f)     { return makeFloat2(f(0), f(1));  }
inline Eigen::Vector2f makeEigVec2f(float2 f) { return Eigen::Vector2f(f.x, f.y); }

inline float3 makeFloat3(float x, float y, float z)
{
    float3 f;
    f.x = x;
    f.y = y;
    f.z = z;
    return f;
}
inline float3 makeFloat3(Eigen::Vector3f f)     { return makeFloat3(f(0), f(1), f(2)); }
inline Eigen::Vector3f makeEigVec3f(float3 f) { return Eigen::Vector3f(f.x, f.y, f.z); }

inline float4 makeFloat4(float x, float y, float z, float w)
{
    float4 f;
    f.x = x;
    f.y = y;
    f.z = z;
    f.w = w;
    return f;
}
inline float4 makeFloat4(Eigen::Vector4f f)     { return makeFloat4(f(0), f(1), f(2), f(3)); }
inline Eigen::Vector4f makeEigVec4f(float4 f) { return Eigen::Vector4f(f.x, f.y, f.z, f.w);  }

} // namespace mh 

#endif /* DATATYPES_H */
