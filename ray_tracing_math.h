#ifndef RAY_TRACING_MATH
#define RAY_TRACING_MATH

#include "math.h"

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

inline v3 unit_vector(v3 a) {
    v3 result = a * (1.0f / length(a));
    return result;
}

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

#endif
