#ifndef RAY_TRACING_CAMERA
#define RAY_TRACING_CAMERA

#include "ray_tracing_math.h"
#include "ray.h"

struct camera {
    v3 pixel00_location;
    v3 pixel_delta_u;
    v3 pixel_delta_v;
    v3 position;         // NOTE(fede): Point camera is looking from
    v3 look_at;          // NOTE(fede): Point camera is looking at
    v3 vup;              // NOTE(fede): Camera relative "up" direction
    v3 u;                // NOTE(fede): unit vector pointing to camera right   
    v3 v;                // NOTE(fede): unit vector pointing to camera up
    v3 w;                // NOTE(fede): unit vector pointing to opposite camera view (right handed)

    float vfov;          // NOTE(fede): Vertical view angle (field of view)
    float focal_length;
};

camera Camera(int image_width, int image_height, v3 position, v3 look_at, v3 vup, float vfov_degrees) {
    camera c = {};
    c.position  = position;
    c.look_at = look_at;
    c.focal_length = length(c.position - c.look_at);
    c.vfov = vfov_degrees;
    c.vup = vup;

    float theta = degrees_to_radians(c.vfov);
    float h = tanf(theta / 2);

    // Viewport setup
    float viewport_height = 2.0 * h * c.focal_length;
    float viewport_width = viewport_height * ((float) image_width / image_height);

    // NOTE(fede): Calculate u,v,w unit basis vectors for the camera coordinate frame
    c.w = normalize(c.position - c.look_at);
    c.u = normalize(cross(c.vup, c.w));
    c.v = cross(c.w, c.u);

    // Viewport
    v3 viewport_u = viewport_width * c.u;
    v3 viewport_v = viewport_height * -c.v;

    v3 pixel_delta_u = viewport_u / image_width;
    v3 pixel_delta_v = viewport_v / image_height;

    v3 viewport_upper_left = c.position - (c.focal_length * c.w) - (viewport_u / 2) - (viewport_v / 2);
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