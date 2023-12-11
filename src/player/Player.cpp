//
// Created by Tony Adriansen on 11/16/23.
//

#include "Player.h"
#include "../input/Keyboard.h"
#include "../Constants.h"
#include <imgui/imgui.h>

Player::Player() = default;

glm::vec3 &Player::getPosition() {
    return position;
}

ChunkKey Player::getChunkKeyPos() const {
    return {static_cast<int>(position.x / CHUNK_WIDTH), static_cast<int>(position.y / CHUNK_WIDTH)};
}

void Player::processKeyInput(GLFWwindow *window, float deltaTime) {
    glm::vec3 &front = camera.getFront();
    glm::vec3 globalUp = Camera::getGlobalUp();

    float multiplier = m_movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * multiplier;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * multiplier;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, globalUp)) * multiplier;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, globalUp)) * multiplier;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        position += globalUp * multiplier;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        position -= globalUp * multiplier;
    }
    camera.setPosition(position);
}


void Player::processMouseInput(GLFWwindow *window, float deltaTime) {
    bool ImGuiWantMouse = ImGui::GetIO().WantCaptureMouse;
    if (!Keyboard::isPressed(GLFW_KEY_B)) {
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

void Player::processScrollInput(double yoffset) {
    if (yoffset > 0) {
        inventory.shiftHotbarSelectedItem(true);
    } else if (yoffset < 0) {
        inventory.shiftHotbarSelectedItem(false);
    }
}

void Player::perFrameUpdate(GLFWwindow *window) {
    if (Keyboard::isPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(window, true);
        return;
    }

    if (Keyboard::isPressedThisFrame(GLFW_KEY_LEFT_BRACKET)) {
        inventory.shiftHotbarSelectedItem(false);
    }

    if (Keyboard::isPressedThisFrame(GLFW_KEY_RIGHT_BRACKET)) {
        inventory.shiftHotbarSelectedItem(true);
    }

    if (Keyboard::isPressedThisFrame(GLFW_KEY_MINUS)) {
        inventory.shiftHotbarStartIndex(false);
    }

    if (Keyboard::isPressedThisFrame(GLFW_KEY_EQUAL)) {
        inventory.shiftHotbarStartIndex(true);
    }
}

float Player::getMovementSpeed() const {
    return m_movementSpeed;
}

void Player::setMovementSpeed(float movementSpeed) {
    Player::m_movementSpeed = movementSpeed;
}
