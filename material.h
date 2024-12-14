#ifndef RAY_TRACING_MATERIAL
#define RAY_TRACING_MATERIAL

#include "hit.h"
#include "ray_tracing_math.h"
#include "ray.h"


typedef enum {
    Lambertian,
    Metal
} Type;

struct material {
    v3 attenuation;
    v3 albedo;
    ray scattered;
    Type type;
};

material lambertian_scatter(ray in, hit h) {
    material result = {};
    v3 scattered_direction = h.normal + rand_unit_vector();
    if (close_to_zero(scattered_direction)) {
        scattered_direction = h.normal;
    }

    ray scattered = { h.p, scattered_direction };
    result.scattered = scattered;
    result.attenuation = h.mat->attenuation;
    result.albedo = h.mat->albedo;

    return result;
}

material scatter(ray in, hit h) {
    material result = {};
    switch (h.mat->type)
    {
    case Lambertian:
        result = lambertian_scatter(in, h);
        break;
    
    default:
        break;
    }

    return result;
}

#endif