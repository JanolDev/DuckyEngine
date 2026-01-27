#pragma once
#include "../math/Vec3.hpp"
#include "../math/Mat4.hpp"
#include "../math/MatrixTransform.hpp"

enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT };

class Camera {
public:
    Vec3 position, front, up, right, worldUp;
    float yaw, pitch, movementSpeed, mouseSensitivity, fov;

    Camera(Vec3 startPos = Vec3(0.0f, 2.0f, 8.0f));
    Mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset, float yoffset);
    void reset(); // Nowa funkcja

private:
    void updateCameraVectors();
};