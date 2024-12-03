#include <stdio.h>
#include <math.h>

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
    v3 red_color = V3(1.0, 0.0, 0.0);

    if (discriminant >= 0) {
        return red_color;
    }

    v3 unit_direction = unit_vector(r.direction);
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

    // Viewport setup
    float viewport_height = 2.0;
    float viewport_width = viewport_height * ((float) image_width / image_height);

    // Camera setup
    // distance between viewport and camera center point
    float focal_length = 1.0f;
    v3 camera_center = V3(0.0, 0.0, 0.0);

    // Viewport
    v3 viewport_u = V3(viewport_width, 0.0, 0.0);
    v3 viewport_v = V3(0.0, -viewport_height, 0.0);

    v3 pixel_delta_u = viewport_u / image_width;
    v3 pixel_delta_v = viewport_v / image_height;

    v3 viewport_upper_left = camera_center - V3(0.0, 0.0, focal_length) - (viewport_u / 2) - (viewport_v / 2);
    v3 pixel00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);


    // Sphere in our scene
    sphere s = { V3(0.0, 0.0, -1.0), 0.5 };

    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; ++j) {
        for (int i = 0; i < image_width; ++i) {
            v3 pixel_center = pixel00_location + (i * pixel_delta_u) + (j * pixel_delta_v);
            v3 ray_direction = pixel_center - camera_center;

            ray r = { camera_center, ray_direction };
            v3 color = ray_color(r, s);

            printf("%d %d %d\n", (int (color.r * 255.999)), (int) (color.g  * 255.999), (int)(color.b  * 255.999));
        }
    }
    return 0;
}