//
// Created by Tony Adriansen on 11/16/23.
//

#include "Player.hpp"
#include "../input/Keyboard.hpp"
#include "../AppConstants.hpp"
#include "../world/block/BlockDB.hpp"

Player::Player() = default;

glm::vec3 &Player::getPosition() {
  return position;
}

void Player::processKeyInput(float deltaTime) {
  const glm::vec3 &front = camera.getFront();
  const glm::vec3 globalUp = Camera::getGlobalUp();

  float multiplier = m_movementSpeed * deltaTime;

  if (Keyboard::isPressed(GLFW_KEY_W))
    position += front * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_S))
    position -= front * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_A))
    position -= glm::normalize(glm::cross(front, globalUp)) * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_D))
    position += glm::normalize(glm::cross(front, globalUp)) * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_R)) {
    position += globalUp * multiplier;
  }
  if (Keyboard::isPressed(GLFW_KEY_F)) {
    position -= globalUp * multiplier;
  }
  camera.setPosition(position);
}

void Player::update(float deltaTime) {
  processKeyInput(deltaTime);
}

void Player::processScrollInput(double yoffset) {
  if (yoffset > 0) {
    inventory.shiftHotbarSelectedItem(true);
  } else if (yoffset < 0) {
    inventory.shiftHotbarSelectedItem(false);
  }
}

void Player::perFrameUpdate() {
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

void Player::renderDebugGui() {
  ImGui::Text("Camera Position  %.2f x  %.2f y %.2f z",
              position.x, position.y, position.z);

  std::string blockName = BlockDB::getBlockData(inventory.getHeldItem()).name;
  ImGui::Text("Block Type: %s", blockName.c_str());
  ImGui::Text("Block ID: %d", static_cast<int>(inventory.getHeldItem()));

  ImGui::SliderFloat("Movement Speed", &m_movementSpeed, 1.0f, 300.0f);
}

void Player::onCursorUpdate(double xOffset, double yOffset) {
  camera.onCursorUpdate(xOffset, yOffset);
}

glm::ivec2 Player::getChunkPosition() {
  return {static_cast<int>(position.x / CHUNK_WIDTH),
          static_cast<int>(position.y / CHUNK_WIDTH)};
}
