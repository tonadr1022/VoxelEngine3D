//
// Created by Tony Adriansen on 11/16/23.
//

#include "Player.h"

Player::Player() = default;

glm::vec3& Player::getPosition() {
    return position;
}
void Player::processKeyInput(GLFWwindow *window) {

    glm::vec3 &front = camera.getFront();
    glm::vec3 globalUp = Camera::getGlobalUp();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(front, globalUp)) * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(front, globalUp)) * movementSpeed;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        position += globalUp * movementSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        position -= globalUp * movementSpeed;
    }
    camera.setPosition(position);
}


void Player::processMouseInput(GLFWwindow *window) {
    bool ImGuiWantMouse = ImGui::GetIO().WantCaptureMouse;
    if (!ImGuiWantMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        camera.processMouseMovement(window);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

}

void Player::update(GLFWwindow *window) {
    processMouseInput(window);
    processKeyInput(window);
    camera.update();
}

//void Player::update() {
//}
