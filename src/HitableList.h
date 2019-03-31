#pragma once

#include "Hitable.h"
#include <vector>

class HitableList : public Hitable
{
public:
    HitableList() {}

    template <typename T, typename ...Args>
    void add(Args&&... args)
    {
        list.emplace_back(new T(std::forward<Args>(args)...));
    }

    bool hit(const ray & r, float t_min, float t_max, hit_record & rec) const override;

    std::vector<Hitable::Ptr> list;
};

bool HitableList::hit(const ray & r, float t_min, float t_max, hit_record & rec) const
{
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (auto& object : list)
    {
        if (object->hit(r, t_min, closest_so_far, temp_rec))
        {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}