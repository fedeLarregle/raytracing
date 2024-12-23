#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "camera.h"
#include "hit.h"
#include "ray_tracing_math.h"
#include "ray.h"
#include "scene.h"

#define array_count(arr) (sizeof((arr)) / (sizeof((arr)[0])))

inline bool surrounds(float min, float max, float n) {
    return min < n && n < max;
}

hit_information ray_color(ray r, float t_min, float t_max, scene scene_object, int object_index) {
    sphere s = scene_object.spheres[object_index];
    v3 cq = s.center - r.origin;
    float a = dot(r.direction, r.direction);
    float b = dot(-2 * r.direction, cq);
    float c = dot(cq, cq) - (s.radius * s.radius);
    // NOTE(fede): Lets evaluate the quadratic equation's
    //  discriminant to see if our ray has hit the sphere
    float discriminant = (b * b) - 4 * a * c;

    if (discriminant >= 0) {
        float t0 = (-b - sqrtf(discriminant)) / (2 * a);

        if (!surrounds(t_min, t_max, t0)) {
            t0 = (-b + sqrtf(discriminant)) / (2 * a);
            if (!surrounds(t_min, t_max, t0)) {
                hit_information result = {};
                result.hit_object = false;

                return result;
            }
        }
        v3 p = ray_at(r, t0);
        v3 outward_normal = normalize(p - s.center);
        bool is_front_face = dot(r.direction, outward_normal) < 0;

        hit_information result = {};
        result.t = t0;
        result.p = p;
        result.normal = is_front_face ? outward_normal : -outward_normal;
        result.hit_object = true;
        result.is_front_face = is_front_face;
        result.object_index = object_index;

        return result;
    }

    hit_information result = {V3(1, 0, 1), FLT_MAX, false};
    return result;
}

v3 ray_color(ray r, int max_depth, float t_min, float t_max, scene s) {
    if (max_depth <= 0) {
        return V3(0.0, 0.0, 0.0);
    }

    int size = 4;
    hit_information closest = {};
    closest.t = FLT_MAX;
    for (int i = 0; i < size; ++i) {
        hit_information h = ray_color(r, t_min, t_max, s, i);
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

    material mat = scatter(r, s, closest);
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
        { V3( 0.0, -100.5, -1.0), 100.0, 0 },
        { V3( 0.0,    0.0, -1.2),   0.5, 1 },
        { V3(-1.0,    0.0, -1.0),   0.5, 2 },
        { V3( 1.0,    0.0, -1.0),   0.5, 3 }
    };

    material materials[4] = { mat1, mat2, mat3, mat4 };

    scene s = { spheres, materials };
    int samples_per_pixel = 100;
    float pixel_samples_scale = 1.0 / samples_per_pixel;
    int max_depth = 50;

    FILE *file = fopen("image_output.ppm", "wb");

    if (file) {
        fprintf(file, "P6\n%d %d\n255\n", image_width, image_height);

        int buffer_size = image_height * image_width * 3;
        char *buffer = (char *) malloc(buffer_size);
        if (buffer == NULL) {
            perror("malloc");
            fclose(file);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < image_height; ++j) {
            for (int i = 0; i < image_width; ++i) {
                v3 color = V3(0.0, 0.0, 0.0);
                // NOTE(fede): Sampling for antialiasing
                for (int sample = 0; sample < samples_per_pixel; ++sample) {
                    ray r = get_ray(c, i, j);
                    color += ray_color(r, max_depth, 0, FLT_MAX, s);
                }

                color *= pixel_samples_scale;
                float r = linear_to_gamma(color.r);
                float g = linear_to_gamma(color.g);
                float b = linear_to_gamma(color.b);
                
                int index = (j * image_width + i) * 3;
                buffer[index + 0] = (int) (r * 255.0f);
                buffer[index + 1] = (int) (g * 255.0f);
                buffer[index + 2] = (int) (b * 255.0f);
            }
        }

        size_t pixel_count = image_width * image_height;
        fwrite(buffer, 3, pixel_count, file);

        fclose(file);

        free(buffer);
    }
    
    return 0;
}