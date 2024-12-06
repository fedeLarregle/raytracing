#include <stdio.h>
#include <math.h>

#include "camera.h"
#include "ray_tracing_math.h"
#include "ray.h"
#include "sphere.h"

v3 ray_color(ray r, sphere s) {
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
            v3 light_direction = normalize(V3(-1.0, 1.0, -1.0));
            float light = max(dot(-light_direction, N), 0.0);

            return clamp01(N0to1 * light);
        }
    }

    v3 unit_direction = normalize(r.direction);
    v3 white_color = V3(1.0, 1.0, 1.0);
    v3 blue_sky_color = V3(0.5, 0.7, 1.0);
    float t = 0.5 * (unit_direction.y + 1.0);

    return lerp(white_color, t, blue_sky_color);
}

int main() {
    float aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int image_height = (int) image_width / aspect_ratio;
    image_height = image_height < 1 ? 1 : image_height;

    camera c = Camera(image_width, image_height);


    // Sphere in our scene
    sphere s = { V3(0.0, 0.0, -1.0), 0.5 };

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            ray r = get_ray(c, i, j);
            v3 color = ray_color(r, s);

            printf("%d %d %d\n", (int) (color.r * 255.0f), (int) (color.g  * 255.0f), (int)(color.b  * 255.0f));
        }
    }
    return 0;
}