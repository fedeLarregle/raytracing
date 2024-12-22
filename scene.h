#ifndef RAY_TRACING_SCENE
#define RAY_TRACING_SCENE

#include "ray_tracing_math.h"
#include "material.h"

struct sphere {
    v3 center;
    float radius;
    material *mat;
};

struct scene {
    // TODO(fede): We can generalize this instead of having a list of _spheres_
    sphere *spheres;
};

#endif