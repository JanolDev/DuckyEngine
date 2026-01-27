#include "Camera.hpp"
#include <cmath>

Camera::Camera(Vec3 startPos) {
    position = startPos;
    movementSpeed = 5.0f;
    mouseSensitivity = 0.12f;
    fov = 45.0f;
    worldUp = Vec3(0.0f, 1.0f, 0.0f);
    reset();
}

void Camera::reset() {
    position = Vec3(0.0f, 2.0f, 8.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    front = Vec3(0.0f, 0.0f, -1.0f);
    updateCameraVectors();
}

Mat4 Camera::getViewMatrix() const {
    return MatrixTransform::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float deltaTime) {
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)  position = position + front * velocity;
    if (direction == BACKWARD) position = position - front * velocity;
    if (direction == LEFT)     position = position - right * velocity;
    if (direction == RIGHT)    position = position + right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset) {
    yaw   += xoffset * mouseSensitivity;
    pitch += yoffset * mouseSensitivity;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    Vec3 nF;
    nF.x = cos(yaw * 0.0174532f) * cos(pitch * 0.0174532f);
    nF.y = sin(pitch * 0.0174532f);
    nF.z = sin(yaw * 0.0174532f) * cos(pitch * 0.0174532f);
    front = nF.normalized();
    right = front.cross(worldUp).normalized();
    up    = right.cross(front).normalized();
}