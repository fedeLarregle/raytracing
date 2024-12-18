#include <stdio.h>
#include <math.h>
#include <float.h>

#include "camera.h"
#include "hit.h"
#include "ray_tracing_math.h"
#include "ray.h"
#include "scene.h"
#include "sphere.h"

#define array_count(arr) (sizeof((arr)) / (sizeof((arr)[0])))

inline bool surrounds(float min, float max, float n) {
    return min < n && n < max;
}

hit ray_color(ray r, float t_min, float t_max, sphere s) {
    v3 cq = s.center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = dot(-2 * r.direction, cq);
    float c = dot(cq, cq) - (s.radius * s.radius);
    // NOTE(fede): Lets evaluate the quadratic equation's
    //  discriminant to see if our ray has hit the sphere
    float discriminant = (b * b) - 4 * a * c;

    if (discriminant >= 0) {
        float t0 = (-b - sqrtf(discriminant)) / (2 * a);
        // NOTE(fede): _fardest_ of the two hits onto the sphere
        // float t1 = (-b + sqrtf(discriminant)) / (2 * a);
        if (!surrounds(t_min, t_max, t0)) {
            t0 = (-b + sqrtf(discriminant)) / (2 * a);
            if (!surrounds(t_min, t_max, t0)) {
                hit result = {};
                result.hit_object = false;

                return result;
            }
        }
        // NOTE(fede): We take the vector going from the
        // center of the sphere to the ray hit point.
        // This will give us the vector pointing perpen-
        // dicular to the hit surface.
        // We normalize that vector for future calculations.
        v3 p = ray_at(r, t0);

        hit result = {};
        result.t = t0;
        result.p = p;
        v3 outward_normal = normalize(p - s.center); // (p - s.center) / s.radius;
        bool is_front_face = dot(r.direction, outward_normal) < 0;
        result.normal = is_front_face ? outward_normal : -outward_normal;
        result.hit_object = true;
        result.mat = s.mat;

        return result;
    }

    hit result = {V3(1, 0, 1), FLT_MAX, false};
    return result;
}

v3 ray_color(ray r, int max_depth, float t_min, float t_max, scene s) {
    if (max_depth <= 0) {
        return V3(0.0, 0.0, 0.0);
    }

    int size = 4;
    hit closest = {};
    closest.t = FLT_MAX;
    for (int i = 0; i < size; ++i) {
        hit h = ray_color(r, t_min, t_max, s.spheres[i]);
        if (h.hit_object && h.t <= closest.t) {
            closest = h;
        }
    }

    if (!closest.hit_object) {
        // NOTE(fede): If no objects hit by ray just render
        //  the _blue_sky_ background for this ray
        v3 unit_direction = normalize(r.direction);
        v3 white_color = V3(1.0, 1.0, 1.0);
        v3 blue_sky_color = V3(0.5, 0.7, 1.0);
        float t = 0.5 * (unit_direction.y + 1.0);

        return lerp(white_color, t, blue_sky_color);
    }

    material mat = scatter(r, closest);
    return hadamard(mat.attenuation, ray_color(mat.scattered, max_depth - 1, 0.001, FLT_MAX, s));
}

inline float linear_to_gamma(float linear_component) {
    if (linear_component > 0) {
        return sqrtf(linear_component);
    }

    return 0;
}

int main() {
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) image_width / aspect_ratio;
    image_height = image_height < 1 ? 1 : image_height;

    camera c = Camera(image_width, image_height);

    material mat1 = {
        .type = Lambertian,
        .attenuation = V3(0.8, 0.8, 0.0),
        .albedo = V3(0.8, 0.8, 0.0),
        .fuzz = 0.0
    };
    material mat2 = {
        .type = Lambertian,
        .attenuation = V3(0.1, 0.2, 0.5),
        .albedo = V3(0.1, 0.2, 0.5),
        .fuzz = 0.0
    };
    material mat3 = {
        .type = Metal,
        .attenuation = V3(0.8, 0.8, 0.8),
        .albedo = V3(0.8, 0.8, 0.8),
        .fuzz = 0.3
    };
    material mat4 = {
        .type = Metal,
        .attenuation = V3(0.8, 0.6, 0.2),
        .albedo = V3(0.8, 0.6, 0.2),
        .fuzz = 1.0
    };

    sphere spheres[4] = {
        { V3( 0.0, -100.5, -1.0), 100.0, &mat1 },
        { V3( 0.0,    0.0, -1.2),   0.5, &mat2 },
        { V3(-1.0,    0.0, -1.0),   0.5, &mat3 },
        { V3( 1.0,    0.0, -1.0),   0.5, &mat4 }
    };

    scene s = {spheres};
    int samples_per_pixel = 100;
    float pixel_samples_scale = 1.0 / samples_per_pixel;
    int max_depth = 50;

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            v3 color = V3(0.0, 0.0, 0.0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(c, i, j);
                color += ray_color(r, max_depth, 0, FLT_MAX, s);
            }

            color *= pixel_samples_scale;
            float r = linear_to_gamma(color.r);
            float g = linear_to_gamma(color.g);
            float b = linear_to_gamma(color.b);
            
            printf("%d %d %d\n", (int) (r * 255.0f), (int) (g  * 255.0f), (int)(b  * 255.0f));
        }
    }
    return 0;
}