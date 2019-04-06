#pragma once

#include "Ray.h"

const float PI = 3.14159265f;

vec3 random_in_unit_disk()
{
    vec3 p;
    do
    {
        p = 2.0f*vec3(get_rand(),get_rand(),0) - vec3(1,1,0);
    } while (dot(p,p) >= 1.0);
    return p;
}


class Camera
{
public:
    Camera(const vec3 & lookfrom, const vec3 & lookat, const vec3 & vup, float vfov, float aspect, float aperture, float focus_dist)
    {
        lens_radius = aperture * 0.5f;
        float theta = vfov*PI/180.f;
        float half_height = tan(theta * 0.5f);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vec(lookfrom - lookat);
        u = unit_vec(cross(vup, w));
        v = cross(w, u);

        left_lower_corner = vec3{-half_width, -half_height, -1.0f};
        left_lower_corner = origin - half_width*focus_dist*u - half_height*focus_dist*v - focus_dist*w;
        horizontal = 2.f * half_width*focus_dist*u;
        vertical = 2.f * half_height*focus_dist*v;
    }

    inline ray get_ray(float u, float v)
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray(origin + offset, left_lower_corner + u * horizontal + v * vertical - origin - offset);
    }

    vec3 left_lower_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u, v, w;
    float lens_radius;
};