#ifndef RAY_TRACING_HIT
#define RAY_TRACING_HIT

#include "ray_tracing_math.h"

struct material;

// TODO(fede): remove bools and make this struct more compact
struct hit {
    v3 color;
    v3 normal;
    v3 p;
    material *mat;
    float t;
    bool hit_object;
    bool is_front_face;
};

#endif