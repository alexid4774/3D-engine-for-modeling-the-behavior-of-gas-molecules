#include "MathCore.hpp"
#include "Camera.hpp"
#include <cmath>



Real toRadians(Real degrees) {
    return degrees * Math::PI / 180.0;
}

Camera::Camera()
{
    this->position = Vec3(0.0, 0.0, 15.0);
    this->worldUp = Vec3(0.0, 1.0, 0.0);
    this->yaw = Real(-90.0);
    this->pitch = Real(0.0);
    this->mouseSensitivity=Real(0.1);
    this->moveSpeed=Real(5.0);
    this->updateCameraVectors();
}

void Camera::updateCameraVectors() {

    Real yawRad = toRadians(yaw);
    Real pitchRad = toRadians(pitch);

    Vec3 f=Vec3(std::cos(yawRad) * std::cos(pitchRad), std::sin(pitchRad), std::sin(yawRad) * std::cos(pitchRad));

    this->front = f.normalized();

    this->right = Vec3::cross(front, worldUp).normalized();

    this->up = Vec3::cross(right, front).normalized();
}

Mat4 Camera::getViewMatrix() const {
    return Mat4::lookAt(this->position, this->position + this->front, this->up);
}

void Camera::moveForward(Real deltaTime) {
    this->position += this->front * (this->moveSpeed * deltaTime);
}
void Camera::moveBackward(Real deltaTime) {
    this->position -= this->front * (this->moveSpeed * deltaTime);
}
void Camera::moveLeft(Real deltaTime) {
    this->position -= this->right * (this->moveSpeed * deltaTime);
}
void Camera::moveRight(Real deltaTime) {
    this->position += this->right * (this->moveSpeed * deltaTime);
}
void Camera::moveUp(Real deltaTime) {
    this->position += this->worldUp * (this->moveSpeed * deltaTime);
}
void Camera::moveDown(Real deltaTime) {
    this->position -= this->worldUp * (this->moveSpeed * deltaTime);
}

void Camera::processMouseMovement(Real xoffset, Real yoffset, bool constrainPitch) {
    xoffset *= this->mouseSensitivity;
    yoffset *= this->mouseSensitivity;

    this->yaw   += xoffset;
    this->pitch += yoffset;

    if (constrainPitch) {
        if (this->pitch > 89.0)  this->pitch = 89.0;
        if (this->pitch < -89.0) this->pitch = -89.0;
    }

    updateCameraVectors();
}

