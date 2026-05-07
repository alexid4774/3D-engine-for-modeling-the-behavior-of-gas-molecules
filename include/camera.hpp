#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "MathCore.hpp"


class Camera
{
public:
        Vec3 position;
        Vec3 front;
        Vec3 up;
        Vec3 right;
        Vec3 worldUp;

        Real yaw;
        Real pitch;

        Real moveSpeed;
        Real mouseSensitivity;
        Camera();
        Mat4 getViewMatrix() const;
        void processMouseMovement(Real xoffset, Real yoffset, bool constrainPitch = true);

        void updateCameraVectors();
        void moveForward(Real deltaTime);
        void moveBackward(Real deltaTime);
        void moveLeft(Real deltaTime);
        void moveRight(Real deltaTime);
        void moveUp(Real deltaTime);
        void moveDown(Real deltaTime);

};

#endif


