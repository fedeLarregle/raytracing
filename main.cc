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
        if (t0 > 0) {
            // NOTE(fede): We take the vector going from the
            // center of the sphere to the ray hit point.
            // This will give us the vector pointing perpen-
            // dicular to the hit surface.
            // We normalize that vector for future calculations.
            v3 N = normalize(ray_at(r, t0) - s.center);
            v3 N0to1 = (0.5 * N) + 0.5;
            // v3 light_direction = normalize(V3(-1.0, 1.0, -1.0));
            // float light = max(dot(-r.direction, N), 0.0);

            hit result = {clamp01(N0to1/* * light */), t0, true};
            return result;
        }
    }

    hit result = {V3(1, 0, 1), FLT_MAX, false};
    return result;
}

v3 ray_color(ray r, float t_min, float t_max, scene s) {
    int size = 2;
    hit closest = {};
    for (int i = 0; i < size; ++i) {
        hit h = ray_color(r, t_min, t_max, s.spheres[i]);
        if (h.hit_object) {
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

    return closest.color;
}

int main() {
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) image_width / aspect_ratio;
    image_height = image_height < 1 ? 1 : image_height;

    camera c = Camera(image_width, image_height);


    // Sphere in our scene
    sphere spheres[2] = {{ V3(0.0, -100.0, -1.0), 100 }, { V3(0.0, 0.0, -1.0), 0.5 }};

    scene s = {spheres};
    int samples_per_pixel = 100;
    float pixel_samples_scale = 1.0 / samples_per_pixel;

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            v3 color = V3(0.0, 0.0, 0.0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                ray r = get_ray(c, i, j);
                color += ray_color(r, 0, FLT_MAX, s);
            }

            color *= pixel_samples_scale;
            
            printf("%d %d %d\n", (int) (color.r * 255.0f), (int) (color.g  * 255.0f), (int)(color.b  * 255.0f));
        }
    }
    return 0;
}