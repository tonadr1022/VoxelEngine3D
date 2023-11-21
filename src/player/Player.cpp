//
// Created by Tony Adriansen on 11/16/23.
//

#include "Player.h"

Player::Player() = default;

glm::vec3& Player::getPosition() {
    return position;
}

std::pair<int, int> Player::getChunkKeyPos() const {
    return {static_cast<int>(position.x / CHUNK_WIDTH), static_cast<int>(position.y / CHUNK_WIDTH)};
}
void Player::processKeyInput(GLFWwindow *window, float deltaTime) {
    glm::vec3 &front = camera.getFront();
    glm::vec3 globalUp = Camera::getGlobalUp();
    // quit on escape
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }
    float multiplier = movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * multiplier;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * multiplier;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, globalUp)) * multiplier;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, globalUp)) * multiplier;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        position += globalUp * multiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        position -= globalUp * multiplier;
    }
    camera.setPosition(position);
}


void Player::processMouseInput(GLFWwindow *window, float deltaTime) {
    bool ImGuiWantMouse = ImGui::GetIO().WantCaptureMouse;
    if (!ImGuiWantMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.processMouseMovement(window, deltaTime);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}

void Player::update(GLFWwindow *window, float deltaTime) {
    processMouseInput(window, deltaTime);
    processKeyInput(window, deltaTime);
    camera.update(deltaTime);
}

//void Player::update() {
//}
