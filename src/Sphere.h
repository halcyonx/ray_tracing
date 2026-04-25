#pragma once

#include "Hitable.h"
#include "Material.h"
#include <memory>

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const vec3 & c, float r, std::shared_ptr<Material> m)
        : center(c)
        , radius(r)
        , material(std::move(m))
    {}

    bool hit(const ray & r, float t_min, float t_max, hit_record & rec) const override;

    vec3 center;
    float radius;
    std::shared_ptr<Material> material;
};

inline bool Sphere::hit(const ray & r, float t_min, float t_max, hit_record & rec) const
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - a*c;

    if (discriminant > 0)
    {
        float sqrt_disc = sqrt(discriminant);
        float temp = (-b - sqrt_disc) / a;
        if (temp > t_min && temp < t_max)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.material = material.get();
            return true;
        }
        temp = (-b + sqrt_disc) / a;
        if (temp > t_min && temp < t_max)
        {
            rec.t = temp;
            rec.p = r.point_at_parameter(temp);
            rec.normal = (rec.p - center) / radius;
            rec.material = material.get();
            return true;
        }
    }
    return false;
}
