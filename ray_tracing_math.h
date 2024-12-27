#ifndef RAY_TRACING_MATH
#define RAY_TRACING_MATH

#include <cstdlib>
#include "math.h"

inline float degrees_to_radians(float degrees) {
    return degrees * M_PI / 180.0;
}

inline float min(float a, float b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

inline float max(float a, float b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }

}

inline float _abs(float a) {
    if (a < 0.0) {
        return -a;
    } else {
        return a;
    }
}

union v3 {
    struct {
        float x, y, z;
    };
    struct {
        float r, g, b;
    };
    float e[3];
};

inline v3 V3(float x, float y, float z) {
    v3 result = { x, y, z };
    return result;
}

inline v3 operator+(v3 a, v3 b) {
    v3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline v3 operator+(v3 a, float b) {
    v3 result;

    result.x = a.x + b;
    result.y = a.y + b;
    result.z = a.z + b;

    return result;
}

inline v3 operator+=(v3 &a, v3 b) {
    a = a + b;
    return a;
}

inline v3 operator-(v3 a, v3 b) {
    v3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline v3 operator-(v3 a) {
    v3 result = { -a.x, -a.y, -a.z };
    return result;
}

inline v3 operator-=(v3 &a, v3 b) {
    a = a - b;
    return a;
}

inline v3 operator*(float a, v3 b) {
    v3 result = { a * b.x, a * b.y, a * b.z };
    return result;
}

inline v3 operator*(v3 a, float b) {
    v3 result = b * a;
    return result;
}

inline v3 hadamard(v3 a, v3 b) {
    v3 result = { a.e[0] * b.e[0], a.e[1] * b.e[1], a.e[2] * b.e[2] };
    return result;
}

inline v3 operator*=(v3 &a, float b) {
    a = a * b;
    return a;
}

inline v3 operator/(v3 a, float t) {
    v3 result = (1/t) * a;
    return result;
}

inline float dot(v3 a, v3 b) {
    float result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline v3 cross(v3 a, v3 b) {
    v3 result = { (a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x) };
    return result;
}

inline float length_squared(v3 a) {
    float result = dot(a, a);
    return result;
}

inline float length(v3 a) {
    float result = sqrtf(length_squared(a));
    return result;
}

inline v3 lerp(v3 a, float t, v3 b) {
    v3 result = (1.0 - t) * a + t * b;
    return result;
}

inline v3 normalize(v3 a) {
    // NOTE(fede): (v3 / |v3|)
    v3 result = a * (1.0f / length(a));
    return result;
}

inline v3 clamp01(v3 a) {
    v3 result = {
        min(max(a.x, 0.0), 1.0),
        min(max(a.y, 0.0), 1.0),
        min(max(a.z, 0.0), 1.0)
    };

    return result;
}

inline float randf() {
    return std::rand() / (RAND_MAX + 1.0);
}

inline float randf(float min, float max) {
    return min + (max - min) * randf();
}

inline v3 randv3() {
    return V3(randf(), randf(), randf());
}

inline v3 randv3(float min, float max) {
    return V3(randf(min, max), randf(min, max), randf(min, max));
}

inline v3 rand_unit_vector() {
    while (true) {
        v3 p = randv3(-1.0, 1.0);
        float lensq = length_squared(p);
        if (1e-160 < lensq && lensq <= 1) {
            return p / sqrtf(lensq);
        }
    }
}

inline v3 random_on_hemisphere(v3 normal) {
    v3 on_unit_sphere = rand_unit_vector();
    if (dot(on_unit_sphere, normal) > 0.0) {
        // NOTE(fede): in the same hemisphere
        return on_unit_sphere;
    } else {
        return -on_unit_sphere;
    }
}

inline bool close_to_zero(v3 a) {
    float threshold = 1e-8;
    return (a.e[0] < threshold) && (a.e[1] < threshold) && (a.e[2] < threshold);
}

inline v3 reflect(v3 v, v3 n) {
    return v - 2 * dot(v, n) * n;
}

inline v3 refract(v3 uv, v3 normal, float refractive_index_ratio) {
    // NOTE(fede): Snell's law
    // n  = refraction index (first  material)
    // n' = refraction index (second material)
    // n' * sinf(refraction angle) = n * sinf(incidence angle) = (n / n') * sinf(incidence angle)
    // index of refraction = n = (speed of light in vacumm) / velocity of light in that medium
    float cos_theta = min(dot(-uv, normal), 1.0);
    v3 ray_out_perpendicular = refractive_index_ratio * (uv + (cos_theta * normal));
    v3 ray_out_parallel = (-sqrtf(_abs(1 - length_squared(ray_out_perpendicular)))) * normal;

    return ray_out_perpendicular + ray_out_parallel;
}

inline float reflectance(float cosine, float refraction_index) {
    // NOTE(fede): Schlick's approximation for reflectance
    float r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 + r0) * powf((1 - cosine), 5);
}

#endif
