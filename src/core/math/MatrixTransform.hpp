#pragma once
#include "Mat4.hpp"
#include "Vec3.hpp"
#include <cmath>

class MatrixTransform {
public:
    static Mat4 translate(const Vec3& v) {
        Mat4 res(1.0f);
        res.m[12] = v.x;
        res.m[13] = v.y;
        res.m[14] = v.z;
        return res;
    }

    static Mat4 scale(const Vec3& v) {
        Mat4 res(1.0f);
        res.m[0] = v.x;
        res.m[5] = v.y;
        res.m[10] = v.z;
        return res;
    }

    static Mat4 rotate(float angle, const Vec3& v) {
        float const a = angle; // Zakładamy radiany, jeśli stopnie to trzeba przekonwertować
        float const c = cos(a);
        float const s = sin(a);

        Vec3 axis = v.normalize();
        Vec3 temp = axis * (1.0f - c);

        Mat4 res;
        res.m[0] = c + temp.x * axis.x;
        res.m[1] = temp.x * axis.y + s * axis.z;
        res.m[2] = temp.x * axis.z - s * axis.y;

        res.m[4] = temp.y * axis.x - s * axis.z;
        res.m[5] = c + temp.y * axis.y;
        res.m[6] = temp.y * axis.z + s * axis.x;

        res.m[8] = temp.z * axis.x + s * axis.y;
        res.m[9] = temp.z * axis.y - s * axis.x;
        res.m[10] = c + temp.z * axis.z;

        res.m[15] = 1.0f;
        return res;
    }

    static Mat4 perspective(float fovy, float aspect, float zNear, float zFar) {
        float const tanHalfFovy = tan(fovy / 2.0f);
        Mat4 res(0.0f);
        res.m[0] = 1.0f / (aspect * tanHalfFovy);
        res.m[5] = 1.0f / (tanHalfFovy);
        res.m[10] = -(zFar + zNear) / (zFar - zNear);
        res.m[11] = -1.0f;
        res.m[14] = -(2.0f * zFar * zNear) / (zFar - zNear);
        return res;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        // ZMIANA: używamy normalize() zamiast normalized()
        Vec3 f = (center - eye).normalize();
        Vec3 s = f.cross(up).normalize();
        Vec3 u = s.cross(f);

        Mat4 res(1.0f);
        res.m[0] = s.x;
        res.m[4] = s.y;
        res.m[8] = s.z;

        res.m[1] = u.x;
        res.m[5] = u.y;
        res.m[9] = u.z;

        res.m[2] = -f.x;
        res.m[6] = -f.y;
        res.m[10] = -f.z;

        // ZMIANA: teraz dot() zadziała, bo dodaliśmy go do Vec3
        res.m[12] = -s.dot(eye);
        res.m[13] = -u.dot(eye);
        res.m[14] = f.dot(eye);

        return res;
    }
};