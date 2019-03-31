#pragma once

#include "Hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const vec3 & c, float r)
        : center(c)
        , radius(r)
    {}

    bool hit(const ray & r, float t_min, float t_max, hit_record & rec) const override;

    vec3 center;
    float radius;
};

bool Sphere::hit(const ray & r, float t_min, float t_max, hit_record & rec) const
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0f * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;

    float t = (discriminant < 0.0f) ? -1.0f 
        :  (-b - sqrt(discriminant)) / (2.0f*a);

    if (t > 0.0) 
    {
        vec3 N = unit_vec(r.point_at_parameter(t) - center);
        rec.t = t;
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = N;
        return true;
    }

    /*if (discriminant > 0)
    {
        float temp = (-b - sqrt(b*b - 4*a*c))/a;
        if (temp > t_min && temp < t_max)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }

        temp = (-b + sqrt(b*b - 4*a*c))/a;
        if (temp > t_min && temp < t_max)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(rec.t);
            rec.normal = (rec.p - center) / radius;
            return true;
        }
    }*/
    return false;
}