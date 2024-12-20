#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
namespace CameraDefaults {
    constexpr float Yaw = -90.f;
    constexpr float Pitch = 0.0f;
    constexpr float Speed = 2.5f;
    constexpr float Sensitivity = 0.1f;
    constexpr float Zoom = 45.f;
}

class Camera {
public:
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;
    glm::vec3 mWorldUp;
    float mYaw;
    float mPitch;
    float mMovementSpeed;
    float mMouseSensitivity;
    float mZoom;

    Camera(glm::vec3 position = glm::vec3(0.f, 0.f, 0.f), 
        glm::vec3 up = glm::vec3(0.f, 1.f, 0.f), 
        float yaw = CameraDefaults::Yaw, 
        float pitch = CameraDefaults::Pitch)
            : mPosition(position)
            , mFront(glm::vec3(0.f, 0.f, -1.f))
            , mWorldUp(up)
            , mYaw(yaw)
            , mPitch(pitch)
            , mMovementSpeed(CameraDefaults::Speed)
            , mMouseSensitivity(CameraDefaults::Sensitivity)
            , mZoom(CameraDefaults::Zoom)
    {
        UpdateCameraVectors();
    }

    Camera(float posX, float posY, float posZ,
        float upX, float upY, float upZ, 
        float yaw = CameraDefaults::Yaw, 
        float pitch = CameraDefaults::Pitch)
            : mPosition(glm::vec3(posX, posY, posZ))
            , mFront(glm::vec3(0.f, 0.f, -1.f))
            , mWorldUp(glm::vec3(upX, upY, upZ))
            , mYaw(yaw)
            , mPitch(pitch)
            , mMovementSpeed(CameraDefaults::Speed)
            , mMouseSensitivity(CameraDefaults::Sensitivity)
            , mZoom(CameraDefaults::Zoom)
    {
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(mPosition, mPosition + mFront, mUp);
    }

    void Move(CameraMovement movement, float deltaTime)
    {
        float velocity = mMovementSpeed * deltaTime;
        if (movement == CameraMovement::FORWARD)
        {
            mPosition += mFront * velocity;
        }
        if (movement == CameraMovement::BACKWARD)
        {
            mPosition -= mFront * velocity;
        }
        if (movement == CameraMovement::LEFT)
        {
            mPosition -= mRight * velocity;
        }
        if (movement == CameraMovement::RIGHT)
        {
            mPosition += mRight * velocity;
        }
    }

    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
    {
        xOffset *= mMouseSensitivity;
        yOffset *= mMouseSensitivity;
        mYaw += xOffset;
        mPitch -= yOffset;
        if (constrainPitch)
        {
            if (mPitch > 89.f)
            {
                mPitch = 89.f;
            }
            if (mPitch < -89.f)
            {
                mPitch = -89.f;
            }
        }
        glm::vec3 direction;
        UpdateCameraVectors();
    }

    void ProcessMouseScroll(float yOffset)
    {
        mZoom -= yOffset;
        if (mZoom < 1.0f)
        {
            mZoom = 1.0f;
        }
        if (mZoom > 45.f)
        {
            mZoom = 45.f;
        }
    }

private:
    void UpdateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        front.y = sin(glm::radians(mPitch));
        front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mFront = glm::normalize(front);
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        mUp = glm::normalize(glm::cross(mRight, mFront));
    }
};