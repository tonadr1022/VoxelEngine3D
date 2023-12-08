//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CAMERA_H
#define VOXEL_ENGINE_CAMERA_H

#include "../physics/Ray.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:

    explicit Camera();

    void processMouseMovement(GLFWwindow *window, float deltaTime);

    void setPosition(glm::vec3 position);

    inline glm::vec3 &getPosition() { return position; }

    inline glm::mat4 &getViewMatrix() { return viewMatrix; }

    inline glm::mat4 &getProjectionMatrix() { return projectionMatrix; }

    inline glm::vec3 &getFront() { return front; }

//    inline glm::vec3 &getRight() { return right; }

//    inline glm::vec3 &getUp() { return up; }

    static inline glm::vec3 &getGlobalUp() { return const_cast<glm::vec3 &>(globalUp); }

    void update(float deltaTime);

//    void setFarPlane(float farPlane);

//    [[nodiscard]] float getFarPlane() const;


private:
    void updateCameraVectors();

    float fov = 70.0f;
    float mouseSensitivity = 0.1f;
    float farPlane = 800.0f;

    constexpr static float aspectRatio = 800.0f / 600.0f;
    constexpr static float nearPlane = 0.1f;
    constexpr static glm::vec3 globalUp = glm::vec3(0.0f, 0.0f, 1.0f);

//    float yaw = -90.0f;
//    float pitch = 0.0f;


    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 eulers = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = globalUp;
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(fov), aspectRatio, nearPlane, farPlane);

};


#endif //VOXEL_ENGINE_CAMERA_H
