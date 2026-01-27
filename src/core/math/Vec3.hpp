#pragma once
#include <cmath>

struct Vec3 {
    float x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

    // Operatory
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }

    // Długość
    float length() const { return std::sqrt(x * x + y * y + z * z); }

    // Normalizacja (Zwraca wektor o długości 1)
    Vec3 normalize() const {
        float len = length();
        if (len == 0) return Vec3(0, 0, 0);
        return Vec3(x / len, y / len, z / len);
    }

    // Iloczyn wektorowy (Cross Product)
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // --- NOWOŚĆ: Iloczyn skalarny (Dot Product) ---
    // Potrzebne do MatrixTransform
    float dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};