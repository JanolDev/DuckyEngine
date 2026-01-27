#include "MatrixTransform.hpp"
#include <cmath>

#define PI 3.1415926535f

Mat4 MatrixTransform::translate(const Vec3& v) {
    Mat4 res;
    res.setIdentity();
    res.m[12] = v.x;
    res.m[13] = v.y;
    res.m[14] = v.z;
    return res;
}

Mat4 MatrixTransform::scale(const Vec3& v) {
    Mat4 res;
    res.setIdentity();
    res.m[0] = v.x;
    res.m[5] = v.y;
    res.m[10] = v.z;
    return res;
}

Mat4 MatrixTransform::rotate(float angleDeg, const Vec3& axis) {
    Mat4 res;
    res.setIdentity();
    float rad = angleDeg * (PI / 180.0f);
    float c = std::cos(rad);
    float s = std::sin(rad);
    float t = 1.0f - c;
    Vec3 a = axis.normalized();

    res.m[0] = t * a.x * a.x + c;
    res.m[1] = t * a.x * a.y + s * a.z;
    res.m[2] = t * a.x * a.z - s * a.y;

    res.m[4] = t * a.x * a.y - s * a.z;
    res.m[5] = t * a.y * a.y + c;
    res.m[6] = t * a.y * a.z + s * a.x;

    res.m[8] = t * a.x * a.z + s * a.y;
    res.m[9] = t * a.y * a.z - s * a.x;
    res.m[10] = t * a.z * a.z + c;

    return res;
}

Mat4 MatrixTransform::perspective(float fovDeg, float aspect, float nearZ, float farZ) {
    Mat4 res;
    for(int i = 0; i < 16; i++) res.m[i] = 0.0f;
    float tanHalfFov = std::tan((fovDeg * PI / 180.0f) / 2.0f);
    res.m[0] = 1.0f / (aspect * tanHalfFov);
    res.m[5] = 1.0f / tanHalfFov;
    res.m[10] = -(farZ + nearZ) / (farZ - nearZ);
    res.m[11] = -1.0f;
    res.m[14] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
    return res;
}

Mat4 MatrixTransform::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).normalized();
    Vec3 s = f.cross(up).normalized();
    Vec3 u = s.cross(f);

    Mat4 res;
    res.setIdentity();
    res.m[0] = s.x;  res.m[4] = s.y;  res.m[8] = s.z;
    res.m[1] = u.x;  res.m[5] = u.y;  res.m[9] = u.z;
    res.m[2] = -f.x; res.m[6] = -f.y; res.m[10] = -f.z;
    res.m[12] = -s.dot(eye);
    res.m[13] = -u.dot(eye);
    res.m[14] = f.dot(eye);
    return res;
}