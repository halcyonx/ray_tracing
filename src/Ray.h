#pragma once
#include "Vec.h"

template <typename T>
class Ray
{
public:
    Ray() {}
    Ray(const vec3 & a, const vec3 & b)
        : A{a}
        , B(b)
    {}

    inline vec3 origin() const { return A; }
    inline vec3 direction() const { return B; }
    inline vec3 point_at_parameter(T t) const { return A + t*B; }

    vec3 A;
    vec3 B;
};

using ray = Ray<float>;