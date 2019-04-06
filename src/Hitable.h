#pragma once

#include "Ray.h"
#include <memory>

class Material;

struct hit_record
{
    float t;
    vec3 p;
    vec3 normal;
    Material * material;
};

class Hitable
{
public:
    using Ptr = std::unique_ptr<Hitable>;
    virtual ~Hitable() {};
    virtual bool hit(const ray & r, float t_min, float t_max, hit_record & rec) const = 0;
};