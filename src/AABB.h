#pragma once
#include "Ray.h"
#include <algorithm>

struct AABB {
    vec3 mn, mx;

    AABB() {}
    AABB(vec3 a, vec3 b) : mn(a), mx(b) {}

    inline bool hit(const ray& r, float t_min, float t_max) const {
        for (int i = 0; i < 3; i++) {
            float inv_d = 1.0f / r.direction()[i];
            float t0 = (mn[i] - r.origin()[i]) * inv_d;
            float t1 = (mx[i] - r.origin()[i]) * inv_d;
            if (inv_d < 0.0f) std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min) return false;
        }
        return true;
    }
};

inline AABB surrounding_box(const AABB& a, const AABB& b) {
    return AABB(
        vec3(fmin(a.mn[0], b.mn[0]), fmin(a.mn[1], b.mn[1]), fmin(a.mn[2], b.mn[2])),
        vec3(fmax(a.mx[0], b.mx[0]), fmax(a.mx[1], b.mx[1]), fmax(a.mx[2], b.mx[2]))
    );
}
