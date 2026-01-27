#pragma once
#include <string>
#include "../math/Vec3.hpp"
#include "../math/Mat4.hpp"
#include "../math/MatrixTransform.hpp"

enum class MeshType { Cube, Triangle, Model, Pyramid };

struct Transform {
    Vec3 position, rotation, scale;
    Transform() : position(0,0,0), rotation(0,0,0), scale(1,1,1) {}
    Mat4 getModelMatrix() const {
        Mat4 t = MatrixTransform::translate(position);
        Mat4 rx = MatrixTransform::rotate(rotation.x, Vec3(1, 0, 0));
        Mat4 ry = MatrixTransform::rotate(rotation.y, Vec3(0, 1, 0));
        Mat4 rz = MatrixTransform::rotate(rotation.z, Vec3(0, 0, 1));
        Mat4 s = MatrixTransform::scale(scale);
        return t * rz * ry * rx * s;
    }
};

struct Material {
    float shininess = 32.0f;
    float specularStrength = 0.5f;
    unsigned int specularMapId = 0;
    std::string specularMapPath;
};

struct SceneObject {
    int id;
    std::string name;
    MeshType type;
    Transform transform;

    // FIZYKA & ROZGRYWKA
    bool hasCollider = true;
    bool useGravity = false;
    bool canShoot = false; // <--- NOWOŚĆ: Czy może strzelać?

    bool lockX = false, lockY = false, lockZ = false;
    Vec3 velocity;

    unsigned int textureId = 0;
    std::string texturePath;
    Material material;
    unsigned int vao = 0;
    int vertexCount = 0;
    std::string modelPath;

    SceneObject() : id(0), name("Object"), type(MeshType::Cube), velocity(0,0,0) {}
};