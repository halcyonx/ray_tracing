#pragma once

#include "Hitable.h"

float get_rand()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

vec3 random_in_unit_sphere()
{
    vec3 p;
    do
    {
        p = 2.0f * vec3(get_rand(), get_rand(), get_rand()) - vec3(1.f, 1.f, 1.f);
    }
    while(p.squared_length() >= 1.f);

    return p;
}

vec3 reflect(const vec3 & v, const vec3 & n)
{
    return v - 2.0f * dot(v, n) * n;
}

bool refract(const vec3 & v, const vec3 n, float ni_over_nt, vec3 & refracted)
{
    vec3 uv = unit_vec(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }
    else 
        return false;
}

float schlick(float cosine, float ref_idx)
{
    float r0 = (1-ref_idx) / (1+ref_idx);
    r0 = r0*r0;
    return r0 + (1-r0)*pow((1 - cosine),5);
}

class Material
{
public:
    virtual ~Material() {}
    virtual bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const = 0;
};


class Lambertian : public Material
{
public:
    Lambertian(const vec3 & v) 
        : albedo(v)
    {}

    bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const override
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p);
        attenuation = albedo;
        return true;
    }

    vec3 albedo;
};


class Metal : public Material
{
public:
    Metal(const vec3 & v, float f)
        : albedo(v)
        , fuzz(f < 1.f ? f : 1.f)
    {}

    bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const override
    {
        vec3 reflected = reflect(unit_vec(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

    vec3 albedo;
    float fuzz;
};


class Dielectric : public Material
{
public:
    Dielectric(float ri)
        : ref_idx(ri)
    {}

    bool scatter(const ray & r_in, const hit_record & rec, vec3 & attenuation, ray & scattered) const override
    {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0); 
        vec3 refracted;
        float reflect_prob;
        float cosine;

        if (dot(r_in.direction(), rec.normal) > 0)
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            //         cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
            cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
        }
        else 
        {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }

        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) 
            reflect_prob = schlick(cosine, ref_idx);
        else 
            reflect_prob = 1.0;
        if (get_rand() < reflect_prob) 
            scattered = ray(rec.p, reflected);
        else 
            scattered = ray(rec.p, refracted);
        return true;
    }

    float ref_idx;
};