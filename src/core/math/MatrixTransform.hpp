#pragma once
#include "Mat4.hpp"
#include "Vec3.hpp"

class MatrixTransform {
public:
    static Mat4 translate(const Vec3& v);
    static Mat4 rotate(float angleDeg, const Vec3& axis);
    static Mat4 scale(const Vec3& v);
    static Mat4 perspective(float fovDeg, float aspect, float nearZ, float farZ);
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);

};