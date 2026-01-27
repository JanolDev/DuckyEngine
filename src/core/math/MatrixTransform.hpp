#pragma once
#include "Mat4.hpp"
#include "Vec3.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class MatrixTransform {
public:
    static Mat4 translate(const Vec3& v) {
        Mat4 res(1.0f);
        res.m[12] = v.x; res.m[13] = v.y; res.m[14] = v.z;
        return res;
    }

    static Mat4 scale(const Vec3& v) {
        Mat4 res(1.0f);
        res.m[0] = v.x; res.m[5] = v.y; res.m[10] = v.z;
        return res;
    }

    static Mat4 rotate(float angleRad, const Vec3& axis) {
        Mat4 res(1.0f);
        float c = cosf(angleRad);
        float s = sinf(angleRad);
        Vec3 a = axis.normalize();
        Vec3 t = a * (1.0f - c);

        res.m[0] = c + t.x * a.x; res.m[1] = t.x * a.y + s * a.z; res.m[2] = t.x * a.z - s * a.y;
        res.m[4] = t.y * a.x - s * a.z; res.m[5] = c + t.y * a.y; res.m[6] = t.y * a.z + s * a.x;
        res.m[8] = t.z * a.x + s * a.y; res.m[9] = t.z * a.y - s * a.x; res.m[10] = c + t.z * a.z;
        return res;
    }

    static Mat4 perspective(float fovDeg, float aspect, float zNear, float zFar) {
        // NAPRAWA: rzutowanie wymaga radianów
        float const fovyRad = fovDeg * (static_cast<float>(M_PI) / 180.0f);
        float const tanHalfFovy = tanf(fovyRad / 2.0f);
        Mat4 res(0.0f);
        res.m[0] = 1.0f / (aspect * tanHalfFovy);
        res.m[5] = 1.0f / (tanHalfFovy);
        res.m[10] = -(zFar + zNear) / (zFar - zNear);
        res.m[11] = -1.0f;
        res.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
        return res;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalize();
        Vec3 s = f.cross(up).normalize();
        Vec3 u = s.cross(f);
        Mat4 res(1.0f);
        res.m[0] = s.x; res.m[4] = s.y; res.m[8] = s.z;
        res.m[1] = u.x; res.m[5] = u.y; res.m[9] = u.z;
        res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
        res.m[12] = -s.dot(eye); res.m[13] = -u.dot(eye); res.m[14] = f.dot(eye);
        return res;
    }
};