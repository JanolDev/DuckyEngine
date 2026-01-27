#pragma once

#include <cmath>
#include "../math/Vec3.hpp"
#include "../math/Mat4.hpp"
#include "../math/MatrixTransform.hpp"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values - teraz constexpr (szybsze i bezpieczniejsze)
constexpr float YAW         = -90.0f;
constexpr float PITCH       =  0.0f;
constexpr float SPEED       =  5.0f;
constexpr float SENSITIVITY =  0.1f;
constexpr float ZOOM        =  45.0f;

class Camera {
public:
    // Camera Attributes
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;

    // Euler Angles
    float yaw;
    float pitch;

    // Camera options
    float movementSpeed;
    float mouseSensitivity;
    float fov;

    Camera(Vec3 position = Vec3(0.0f, 0.0f, 0.0f), Vec3 up = Vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : front(Vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), fov(ZOOM) {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }

    // Dodano 'const' - ta metoda nie zmienia stanu kamery
    Mat4 getViewMatrix() const {
        return MatrixTransform::lookAt(position, position + front, up);
    }

    void processKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)  position = position + front * velocity;
        if (direction == BACKWARD) position = position - front * velocity;
        if (direction == LEFT)     position = position - right * velocity;
        if (direction == RIGHT)    position = position + right * velocity;
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)  pitch = 89.0f;
            if (pitch < -89.0f) pitch = -89.0f;
        }

        updateCameraVectors();
    }


    void updateCameraVectors() {
        Vec3 newFront;
        newFront.x = std::cos(toRadians(yaw)) * std::cos(toRadians(pitch));
        newFront.y = std::sin(toRadians(pitch));
        newFront.z = std::sin(toRadians(yaw)) * std::cos(toRadians(pitch));


        front = newFront.normalize();

        right = front.cross(worldUp).normalize();
        up    = right.cross(front).normalize();
    }

private:
    // Dodano 'static', bo funkcja nie używa zmiennych klasy
    static float toRadians(float degree) {
        return degree * (3.14159265359f / 180.0f);
    }
};