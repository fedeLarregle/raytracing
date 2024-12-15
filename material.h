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
    Type type;
    v3 attenuation;
    v3 albedo;
    ray scattered;
    float fuzz;
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

material metal_scatter(ray in, hit h) {
    material result = {};

    v3 reflected = reflect(in.direction, h.normal);
    reflected = normalize(reflected) + (h.mat->fuzz * rand_unit_vector());
    ray scattered = { h.p, reflected };
    if (dot(scattered.direction, h.normal) < 0) {
        result.albedo = V3(0.0, 0.0, 0.0);
        result.attenuation = V3(0.0, 0.0, 0.0);
    } else {
        result.albedo = h.mat->albedo;
        result.attenuation = h.mat->attenuation;
    }

    result.scattered = scattered;

    return result;
}

material scatter(ray in, hit h) {
    material result = {};
    switch (h.mat->type)
    {
    case Lambertian: {
        result = lambertian_scatter(in, h);
        break;
    }
    case Metal: {
        result = metal_scatter(in, h);
        break;
    }
    
    default:
        break;
    }

    return result;
}

#endif