//
// Created by Tony Adriansen on 11/16/23.
//

#include "Camera.hpp"
#include "../AppConstants.hpp"

void Camera::onCursorUpdate(double xOffset, double yOffset) {
    auto dEulers = glm::vec3(0.0f);
    dEulers.z = static_cast<float>(mouseSensitivity * (1- xOffset));
    dEulers.y = static_cast<float>(mouseSensitivity * (yOffset));

    eulers += dEulers;
    eulers.y = glm::clamp(eulers.y, -89.0f, 89.0f);
    eulers.z = glm::mod(eulers.z, 360.0f);

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
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
    updateCameraVectors();
}

void Camera::setPosition(glm::vec3 newPosition) {
    this->position = newPosition;
}

void Camera::updateProjectionMatrix(float aspectRatio) {
    projectionMatrix = glm::perspective(glm::radians(fov),
                                        aspectRatio,
                                        nearPlane, farPlane);
}

Camera::Camera() = default;


