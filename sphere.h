#ifndef RAY_TRACING_SPHERE
#define RAY_TRACING_SPHERE

#include "ray_tracing_math.h"
#include "material.h"

struct sphere {
    v3 center;
    float radius;
    material *mat;
};

#endif