#ifndef RAY_TRACING_RAY
#define RAY_TRACING_RAY

#include "ray_tracing_math.h"

struct ray {
    v3 origin;
    v3 direction;
};

v3 ray_at(ray r, float t) {
    v3 result = r.origin + r.direction * t;

    return result;
}

#endif