#pragma once
#include <string>
#include <vector>
#include "../math/Vec3.hpp"
#include "../math/Mat4.hpp"

// Typy obiektów 3D
enum class MeshType {
    Triangle = 0,
    Quad = 1,
    Pyramid = 2,
    Cube = 3
};

struct Transform {
    Vec3 position = {0.0f, 0.0f, 0.0f};
    Vec3 rotation = {0.0f, 0.0f, 0.0f};
    Vec3 scale = {1.0f, 1.0f, 1.0f};

    // Definicja inline - to jest jedyne miejsce, gdzie ta funkcja powinna istnieć
    Mat4 getModelMatrix() const {
        Mat4 translation = Mat4::translate(position);
        Mat4 rotX = Mat4::rotate(rotation.x, Vec3(1, 0, 0));
        Mat4 rotY = Mat4::rotate(rotation.y, Vec3(0, 1, 0));
        Mat4 rotZ = Mat4::rotate(rotation.z, Vec3(0, 0, 1));
        Mat4 scaleMat = Mat4::scale(scale);

        return translation * (rotZ * rotY * rotX) * scaleMat;
    }
};

struct SceneObject {
    int id;
    std::string name;
    MeshType type;
    Transform transform;
};