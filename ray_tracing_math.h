#ifndef RAY_TRACING_MATH
#define RAY_TRACING_MATH

#include <cstdlib>
#include "math.h"

float min(float a, float b) {
    if (a < b) {
        return a;
    } else {
        return b;
    }
}

float max(float a, float b) {
    if (a > b) {
        return a;
    } else {
        return b;
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
    v3 result = (1 - t) * a + t * b;
    return result;
}

inline v3 normalize(v3 a) {
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

#endif
