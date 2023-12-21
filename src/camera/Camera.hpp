//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CAMERA_HPP
#define VOXEL_ENGINE_CAMERA_HPP

#include "../physics/Ray.hpp"
#include "../EngineConfig.hpp"
#include "../AppConstants.hpp"

class Camera {
public:

    explicit Camera();

    void onCursorUpdate(double xOffset, double yOffset);

    void setPosition(glm::vec3 position);

    void updateViewMatrix();

    void updateProjectionMatrix(float aspectRatio);

    inline glm::vec3 &getPosition() { return m_position; }

    inline glm::mat4 &getViewMatrix() { return m_viewMatrix; }

    inline glm::mat4 &getProjectionMatrix() { return m_projectionMatrix; }

    inline glm::vec3 &getFront() { return m_front; }

    static inline glm::vec3 &getGlobalUp() { return const_cast<glm::vec3 &>(globalUp); }

private:
    void updateCameraVectors();

    float m_fov = 70.0f;
    float m_mouseSensitivity = 0.1f;

    constexpr static float FAR_PLANE = 800.0f;
    constexpr static float NEAR_PLANE = 0.1f;
    constexpr static glm::vec3 globalUp = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);

    float m_yaw_deg = -90.0f;
    float m_pitch_deg = 0.0f;

    glm::vec3 m_front = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_up = globalUp;
    glm::vec3 m_right = glm::normalize(glm::cross(m_front, m_up));

    glm::mat4 m_viewMatrix = glm::mat4(1.0f);
    glm::mat4 m_projectionMatrix = glm::perspective(
            glm::radians(m_fov),
            static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
            NEAR_PLANE, FAR_PLANE);
};


#endif //VOXEL_ENGINE_CAMERA_HPP
