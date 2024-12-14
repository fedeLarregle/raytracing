#ifndef RAY_TRACING_HIT
#define RAY_TRACING_HIT

#include "ray_tracing_math.h"

struct material;

struct hit {
    v3 color;
    v3 normal;
    v3 p;
    material *mat;
    float t;
    bool hit_object;
};

#endif