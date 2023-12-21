//
// Created by Tony Adriansen on 11/16/23.
//

#include "Camera.hpp"
#include "../AppConstants.hpp"

void Camera::onCursorUpdate(double xOffset, double yOffset) {
    m_yaw_deg = glm::mod(m_yaw_deg + static_cast<float>(-xOffset) * m_mouseSensitivity, 360.0f);
    m_pitch_deg = glm::clamp(m_pitch_deg + static_cast<float>(yOffset) * m_mouseSensitivity, -89.0f,
                           89.0f);
    updateCameraVectors();
}

void Camera::setPosition(glm::vec3 newPosition) {
    m_position = newPosition;
    updateViewMatrix();
}

void Camera::updateProjectionMatrix(float aspectRatio) {
    m_projectionMatrix = glm::perspective(glm::radians(m_fov),
                                        aspectRatio,
                                        NEAR_PLANE, FAR_PLANE);
}

void Camera::updateCameraVectors() {
    float yaw_rad = glm::radians(m_yaw_deg);
    float pitch_rad = glm::radians(m_pitch_deg);
    m_front = glm::normalize(glm::vec3{cos(yaw_rad) * cos(pitch_rad),
                                     sin(yaw_rad) * cos(pitch_rad),
                                     sin(pitch_rad)});
    m_right = glm::normalize(glm::cross(m_front, globalUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

Camera::Camera() = default;


