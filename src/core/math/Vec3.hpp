#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    // Podstawowe operacje
    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(float f) const { return Vec3(x * f, y * f, z * f); }
    void operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; }
    void operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; }

    // Matematyka wektorowa
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }

    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    float length() const { return std::sqrt(x * x + y * y + z * z); }

    Vec3 normalized() const {
        float l = length();
        if (l > 0) return (*this) * (1.0f / l);
        return Vec3(0, 0, 0);
    }
};