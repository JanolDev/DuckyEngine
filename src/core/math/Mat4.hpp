#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include "Vec3.hpp"

class Mat4 {
public:
    // Tablica 16-elementowa (4x4) w układzie kolumnowym dla OpenGL
    float m[16];

    // Konstruktor: domyślnie tworzy macierz jednostkową (Identity Matrix)
    Mat4() {
        setIdentity();
    }

    // Ustawia macierz jednostkową
    void setIdentity() {
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = 1.0f; m[5] = 1.0f; m[10] = 1.0f; m[15] = 1.0f;
    }

    // Metoda, której brakowało w Rendererze - zwraca surowy wskaźnik do danych
    const float* data() const {
        return m;
    }

    // Operator mnożenia macierzy (niezbędny do łączenia transformacji)
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {       // kolumna
            for (int j = 0; j < 4; j++) {   // wiersz
                result.m[i * 4 + j] =
                    m[0 * 4 + j] * other.m[i * 4 + 0] +
                    m[1 * 4 + j] * other.m[i * 4 + 1] +
                    m[2 * 4 + j] * other.m[i * 4 + 2] +
                    m[3 * 4 + j] * other.m[i * 4 + 3];
            }
        }
        return result;
    }

    // Statyczne metody pomocnicze do tworzenia macierzy transformacji
    static Mat4 translate(const Vec3& v) {
        Mat4 res;
        res.m[12] = v.x;
        res.m[13] = v.y;
        res.m[14] = v.z;
        return res;
    }

    static Mat4 scale(const Vec3& v) {
        Mat4 res;
        res.m[0] = v.x;
        res.m[5] = v.y;
        res.m[10] = v.z;
        return res;
    }

    // Dodaj te metody, jeśli ich nie masz w MatrixTransform
    static Mat4 rotate(float angleDeg, const Vec3& axis) {
        Mat4 res;
        float angleRad = angleDeg * (3.14159f / 180.0f);
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        float t = 1.0f - c;

        Vec3 a = axis; // upewnij się, że oś jest znormalizowana

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
};