//
// Created by Tony Adriansen on 11/16/23.
//

#include "Camera.h"


//Camera::Camera(GLFWwindow *window) : window(window) {}

void Camera::processMouseMovement(GLFWwindow *window, float deltaTime) {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    glfwSetCursorPos(window, WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    auto dEulers = glm::vec3(0.0f);
    dEulers.z = mouseSensitivity * (WINDOW_WIDTH / 2.0f - xPos);
    dEulers.y = mouseSensitivity * (WINDOW_HEIGHT / 2.0f - yPos);

    eulers += dEulers;
    eulers.y = glm::clamp(eulers.y, -89.0f, 89.0f);
    eulers.z = glm::mod(eulers.z, 360.0f);

    updateCameraVectors(deltaTime);
}

void Camera::updateCameraVectors(float deltaTime) {
    float theta = glm::radians(eulers.z);
    float phi = glm::radians(eulers.y);

    front.x = cos(theta) * cos(phi);
    front.y = sin(theta) * cos(phi);
    front.z = sin(phi);
    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, globalUp));
    up = glm::normalize(glm::cross(right, front));

    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::update(float deltaTime) {
    updateCameraVectors(deltaTime);
}

void Camera::setPosition(glm::vec3 newPosition) {
    this->position = newPosition;
}

void Camera::setFarPlane(float newFarPlane) {
    farPlane = newFarPlane;

}

float Camera::getFarPlane() const {
    return farPlane;
}

Camera::Camera()= default;


