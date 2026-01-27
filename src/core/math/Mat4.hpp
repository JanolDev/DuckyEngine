#pragma once
#include <algorithm> // std::fill

struct Mat4 {
    float m[16];

    // Domyślny konstruktor - Macierz jednostkowa (Identity)
    Mat4() {
        std::fill(m, m + 16, 0.0f);
        m[0] = 1.0f; m[5] = 1.0f; m[10] = 1.0f; m[15] = 1.0f;
    }

    // --- NOWOŚĆ: Konstruktor z wartością (Naprawia błędy w MatrixTransform) ---
    Mat4(float val) {
        std::fill(m, m + 16, 0.0f);
        // Jeśli 1.0f, tworzymy macierz jednostkową (Identity)
        // Jeśli 0.0f, tworzymy macierz zerową (Zero)
        if (val != 0.0f) {
            m[0] = val; m[5] = val; m[10] = val; m[15] = val;
        }
    }

    const float* data() const { return m; }
    float* data() { return m; }

    // Mnożenie macierzy (Column-Major order dla OpenGL)
    Mat4 operator*(const Mat4& other) const {
        Mat4 res(0.0f);
        for (int c = 0; c < 4; c++) {
            for (int r = 0; r < 4; r++) {
                float sum = 0.0f;
                for (int k = 0; k < 4; k++) {
                    sum += m[k * 4 + r] * other.m[c * 4 + k];
                }
                res.m[c * 4 + r] = sum;
            }
        }
        return res;
    }
};