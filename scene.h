#ifndef RAY_TRACING_SCENE
#define RAY_TRACING_SCENE

#include "ray_tracing_math.h"

typedef enum {
    Lambertian,
    Metal,
    Dielectric
} Type;

struct material {
    Type type;
    v3 attenuation;
    v3 albedo;
    ray scattered;
    float fuzz;
    float refraction_index;
};

struct sphere {
    v3 center;
    float radius;
    int material_index;
};

struct scene {
    // TODO(fede): We can generalize this instead of having a list of _spheres_
    sphere *spheres;
    size_t sphere_count;
    material *materials;
    size_t material_count;
};

material lambertian_scatter(ray* in, hit_information* h, material* mat) {
    material result = {};
    v3 scattered_direction = h->normal + rand_unit_vector();
    if (close_to_zero(scattered_direction)) {
        scattered_direction = h->normal;
    }

    ray scattered = { h->p, scattered_direction };
    result.scattered = scattered;
    result.attenuation = mat->attenuation;
    result.albedo = mat->albedo;

    return result;
}

material metal_scatter(ray* in, hit_information* h, material* mat) {
    material result = {};

    v3 reflected = reflect(in->direction, h->normal);
    reflected = normalize(reflected) + (mat->fuzz * rand_unit_vector());
    ray scattered = { h->p, reflected };
    if (dot(scattered.direction, h->normal) < 0) {
        result.albedo = V3(0.0, 0.0, 0.0);
        result.attenuation = V3(0.0, 0.0, 0.0);
    } else {
        result.albedo = mat->albedo;
        result.attenuation = mat->attenuation;
    }

    result.scattered = scattered;

    return result;
}

material dielectric_scatter(ray* in, hit_information* h, material* mat) {
    material result = {};

    float refractive_index = h->is_front_face ? (1.0 / mat->refraction_index) : mat->refraction_index;
    // NOTE(fede): Little math remainder
    //  a . b = |a| |b| cos_theta
    //  if we use unit vectors we can simplify to:
    //  a . b = cos_theta
    v3 unit_direction = normalize(in->direction);
    float cos_theta = min(dot(-unit_direction, h->normal), 1.0);
    float sin_theta = sqrtf(1 - (cos_theta * cos_theta));
    bool cannot_refract = refractive_index * sin_theta > 1.0;

    v3 direction = {};
    if (cannot_refract || reflectance(cos_theta, mat->refraction_index) > randf()) {
        direction = reflect(unit_direction, h->normal);
    } else {
        direction = refract(unit_direction, h->normal, refractive_index);
    }

    ray scattered = { h->p, direction };
    result.attenuation = V3(1.0, 1.0, 1.0);
    result.scattered = scattered;

    return result;
}

material scatter(ray* in, scene* scene_object, hit_information* h) {
    material result = {};
    material mat = scene_object->materials[scene_object->spheres[h->object_index].material_index];
    switch (mat.type)
    {
    case Lambertian: {
        result = lambertian_scatter(in, h, &mat);
        break;
    }
    case Metal: {
        result = metal_scatter(in, h, &mat);
        break;
    }
    case Dielectric: {
        result = dielectric_scatter(in, h, &mat);
        break;
    }
    
    default:
        break;
    }

    return result;
}

#endif