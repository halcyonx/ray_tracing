#pragma once

#include "Ray.h"

class Camera
{
public:
    Camera()
        : left_lower_corner(-2.0f, -1.0f, -1.0f)
        , horizontal(4.0f, 0.f, 0.f)
        , vertical(0.f, 2.f, 0.f)
        , origin(0.f, 0.f, 0.f)
    {}

    inline ray get_ray(float u, float v)
    {
        return ray(origin, left_lower_corner + u * horizontal + v * vertical - origin);
    }

    vec3 left_lower_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
};