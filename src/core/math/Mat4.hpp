#pragma once
#include <algorithm>

struct Mat4 {
    float m[16];

    // Identity matrix
    Mat4() {
        std::fill(m, m + 16, 0.0f);
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }

    // Konstruktor diagonalny (Naprawia błędy inicjalizacji)
    explicit Mat4(float val) {
        std::fill(m, m + 16, 0.0f);
        if (val != 0.0f) {
            m[0] = m[5] = m[10] = m[15] = val;
        }
    }

    const float* data() const { return m; }
    float* data() { return m; }

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