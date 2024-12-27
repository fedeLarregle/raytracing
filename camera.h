#ifndef RAY_TRACING_CAMERA
#define RAY_TRACING_CAMERA

#include "ray_tracing_math.h"
#include "ray.h"

struct camera {
    v3 pixel00_location;
    v3 pixel_delta_u;
    v3 pixel_delta_v;
    v3 position;            // NOTE(fede): Point camera is looking from
    v3 direction;           // NOTE(fede): Point camera is looking at

    float vfov;             // NOTE(fede): Vertical view angle (field of view)
    float focal_length;
};

camera Camera(int image_width, int image_height) {
    float focal_length = 1.0f;

    camera c = {};
    c.position  = V3(0.0, 0.0, 0.0);
    c.direction = V3(0.0, 0.0, -1.0);
    c.focal_length = focal_length;
    c.vfov = 90.0;

    v3 camera_up_direction = V3(0.0, 1.0, 0.0);
    float theta = degrees_to_radians(c.vfov);
    float h = tanf(theta / 2);

    // Viewport setup
    float viewport_height = 2.0 * h * c.focal_length;
    float viewport_width = viewport_height * ((float) image_width / image_height);

    // Viewport
    v3 viewport_u = V3(viewport_width, 0.0, 0.0);
    v3 viewport_v = V3(0.0, -viewport_height, 0.0);

    v3 pixel_delta_u = viewport_u / image_width;
    v3 pixel_delta_v = viewport_v / image_height;

    v3 viewport_upper_left = c.position - V3(0.0, 0.0, focal_length) - (viewport_u / 2) - (viewport_v / 2);
    v3 pixel00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    c.pixel00_location = pixel00_location;
    c.pixel_delta_u = pixel_delta_u;
    c.pixel_delta_v = pixel_delta_v;

    return c;
}

ray get_ray(camera c, int x, int y) {
    v3 random_offset = V3(randf() - 0.5, randf() - 0.5, 0.0);
    v3 pixel_center = c.pixel00_location + 
                      ((x + random_offset.x) * c.pixel_delta_u) +
                      ((y + random_offset.y) * c.pixel_delta_v);
    v3 ray_direction = pixel_center - c.position;
    ray r = { c.position, ray_direction };

    return r;
}

#endif