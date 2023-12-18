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

    void updateProjectionMatrix(float aspectRatio);

    inline glm::vec3 &getPosition() { return position; }

    inline glm::mat4 &getViewMatrix() { return viewMatrix; }

    inline glm::mat4 &getProjectionMatrix() { return projectionMatrix; }

    inline glm::vec3 &getFront() { return front; }

//    inline glm::vec3 &getRight() { return right; }

//    inline glm::vec3 &getUp() { return up; }

    static inline glm::vec3 &getGlobalUp() { return const_cast<glm::vec3 &>(globalUp); }

    void update(float deltaTime);


private:
    void updateCameraVectors();

    float fov = 70.0f;
    float mouseSensitivity = 0.1f;
    float farPlane = 800.0f;

    constexpr static float nearPlane = 0.1f;
    constexpr static glm::vec3 globalUp = glm::vec3(0.0f, 0.0f, 1.0f);

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 eulers = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = globalUp;
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(fov),
            static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
            nearPlane, farPlane);
};


#endif //VOXEL_ENGINE_CAMERA_HPP
