#ifndef RAY_TRACING_CAMERA
#define RAY_TRACING_CAMERA

#include "ray_tracing_math.h"
#include "ray.h"

struct camera {
    v3 pixel00_location;
    v3 pixel_delta_u;
    v3 pixel_delta_v;
    v3 position;            // NOTE(fede): Point camera is looking from
    v3 look_at;             // NOTE(fede): Point camera is looking at
    v3 vup;                 // NOTE(fede): Camera relative "up" direction
    v3 u;                   // NOTE(fede): unit vector pointing to camera right   
    v3 v;                   // NOTE(fede): unit vector pointing to camera up
    v3 w;                   // NOTE(fede): unit vector pointing to opposite camera view (right handed)
    v3 defocus_disk_u;      // NOTE(fede): defocus disk horizontal radius
    v3 defocus_disk_v;      // NOTE(fede): defocus disk vertical radius
    float defocus_angle;    // NOTE(fede): variation angle of rays through each pixel
    float focus_distance;   // NOTE(fede): distance from camera position point to plane of perfect focus
    float vfov;             // NOTE(fede): Vertical view angle (field of view)
};

camera Camera(int image_width, int image_height, v3 position, v3 look_at, v3 vup, float vfov_degrees, float focus_distance, float defocus_angle_degrees) {
    camera c = {};
    c.position  = position;
    c.look_at = look_at;
    c.focus_distance = focus_distance;// length(c.position - c.look_at);
    c.vfov = vfov_degrees;
    c.vup = vup;
    c.defocus_angle = defocus_angle_degrees;

    float theta = degrees_to_radians(c.vfov);
    float h = tanf(theta / 2);

    // Viewport setup
    float viewport_height = 2.0 * h * c.focus_distance;
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

    v3 viewport_upper_left = c.position - (c.focus_distance * c.w) - (viewport_u / 2) - (viewport_v / 2);
    v3 pixel00_location = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    c.pixel00_location = pixel00_location;
    c.pixel_delta_u = pixel_delta_u;
    c.pixel_delta_v = pixel_delta_v;

    // NOTE(fede): Camera defocus disk basis vectors
    float defocus_radius = c.focus_distance * tanf(degrees_to_radians(c.defocus_angle / 2));
    c.defocus_disk_u = c.u * defocus_radius;
    c.defocus_disk_v = c.v * defocus_radius;
    return c;
}

inline v3 defocus_disk_sample(camera * c) {
    v3 p = rand_in_unit_disk();
    return c->position + (p.e[0] * c->defocus_disk_u) + (p.e[1] * c->defocus_disk_v);
}

ray get_ray(camera c, int x, int y) {
    v3 random_offset = V3(randf() - 0.5, randf() - 0.5, 0.0);
    v3 pixel_center = c.pixel00_location + 
                      ((x + random_offset.x) * c.pixel_delta_u) +
                      ((y + random_offset.y) * c.pixel_delta_v);
    v3 ray_origin = (c.defocus_angle <= 0) ? c.position : defocus_disk_sample(&c);
    v3 ray_direction = pixel_center - ray_origin;
    ray r = { ray_origin, ray_direction };

    return r;
}

#endif