#ifndef RAY_TRACING_HIT
#define RAY_TRACING_HIT

#include "ray_tracing_math.h"

struct hit {
    v3 color;
    v3 normal;
    v3 p;
    float t;
    bool hit_object;
};

#endif