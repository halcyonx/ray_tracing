#pragma once
#include "Hitable.h"
#include "AABB.h"
#include <vector>
#include <algorithm>

class BVHNode : public Hitable
{
public:
    BVHNode(std::vector<Hitable::Ptr>& objects, int start, int end, int depth = 0);

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    bool bounding_box(AABB& out) const override { out = box; return true; }

private:
    Hitable::Ptr left, right;
    AABB box;
};

inline BVHNode::BVHNode(std::vector<Hitable::Ptr>& objects, int start, int end, int depth)
{
    int axis = depth % 3;
    auto comparator = [axis](const Hitable::Ptr& a, const Hitable::Ptr& b) {
        AABB ba, bb;
        a->bounding_box(ba);
        b->bounding_box(bb);
        return ba.mn[axis] < bb.mn[axis];
    };

    int span = end - start;
    if (span == 1) {
        left = std::move(objects[start]);
        right = nullptr;
        left->bounding_box(box);
    } else if (span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left  = std::move(objects[start]);
            right = std::move(objects[start + 1]);
        } else {
            left  = std::move(objects[start + 1]);
            right = std::move(objects[start]);
        }
        AABB lb, rb;
        left->bounding_box(lb);
        right->bounding_box(rb);
        box = surrounding_box(lb, rb);
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);
        int mid = start + span / 2;
        left  = std::make_unique<BVHNode>(objects, start, mid, depth + 1);
        right = std::make_unique<BVHNode>(objects, mid,   end, depth + 1);
        AABB lb, rb;
        left->bounding_box(lb);
        right->bounding_box(rb);
        box = surrounding_box(lb, rb);
    }
}

inline bool BVHNode::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
    if (!box.hit(r, t_min, t_max)) return false;

    bool hit_left  = left  && left->hit(r, t_min, t_max, rec);
    bool hit_right = right && right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
    return hit_left || hit_right;
}
