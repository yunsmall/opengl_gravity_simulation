//
// Created by 91246 on 2024/4/9.
//

#include "Camera.h"

void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 tmp_front;
    tmp_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    tmp_front.y = sin(glm::radians(pitch));
    tmp_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(tmp_front);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, front));

}

void Camera::ProcessMouseScroll(float yoffset)
{
    fov -= (float)yoffset;
    if (fov < 0.5f)
        fov = 0.5f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.5f)
            pitch = 89.5f;
        if (pitch < -89.5f)
            pitch = -89.5f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = speed * deltaTime;
    if (direction == FORWARD)
        position += front * velocity;
    else if (direction == BACKWARD)
        position -= front * velocity;
    else if (direction == LEFT)
        position -= right * velocity;
    else if (direction == RIGHT)
        position += right * velocity;
    else if (direction == UP)
        position += up * velocity;
    else if (direction == DOWN)
        position -= up * velocity;
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(DEFAULT_SPEED), sensitivity(DEFAULT_SENSITIVITY), fov(DEFAULT_FOV)
{
    this->position = glm::vec3(posX, posY, posZ);
    this->worldUp = glm::vec3(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), speed(DEFAULT_SPEED), sensitivity(DEFAULT_SENSITIVITY), fov(DEFAULT_FOV)
{
    this->position = position;
    worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}
