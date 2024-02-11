//
// Created by Tony Adriansen on 11/16/23.
//

#include "Player.hpp"
#include "../input/Keyboard.hpp"
#include "../world/block/BlockDB.hpp"
#include "../world/world.hpp"
#include "../input/Mouse.hpp"
#include "../application/Window.hpp"

Player::Player(World* world, Window& window) : m_world(world), m_window(window) {}

glm::vec3& Player::getPosition() {
  return position;
}

void Player::setFocus(bool focus) {
  m_focus = focus;
}

void Player::processKeyInput(double deltaTime) {
  const glm::vec3& front = camera.getFront();
  const glm::vec3 globalUp = Camera::getGlobalUp();
//  std::cout << front.x << " " << front.y << " " << front.z << std::endl;
  auto multiplier = static_cast<float>(m_movementSpeed * deltaTime);

  if (Keyboard::isPressed(GLFW_KEY_W) || Keyboard::isPressed(GLFW_KEY_O))
    position += front * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_S) || Keyboard::isPressed(GLFW_KEY_L))
    position -= front * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_A) || Keyboard::isPressed(GLFW_KEY_K))
    position -= glm::normalize(glm::cross(front, globalUp)) * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_D) || Keyboard::isPressed(GLFW_KEY_SEMICOLON))
    position += glm::normalize(glm::cross(front, globalUp)) * multiplier;
  if (Keyboard::isPressed(GLFW_KEY_R) || Keyboard::isPressed(GLFW_KEY_U)) {
    position += globalUp * multiplier;
  }
  if (Keyboard::isPressed(GLFW_KEY_F) || Keyboard::isPressed(GLFW_KEY_J)) {
    position -= globalUp * multiplier;
  }
  camera.setPosition(position);
}

void Player::update(double deltaTime) {
  processKeyInput(deltaTime);
  perFrameUpdate();
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

  if (Keyboard::isPressedThisFrame(GLFW_KEY_ESCAPE)) {
    m_focus = !m_focus;
    if (m_focus) {
      m_window.lockCursor();
      m_window.centerCursor();
    } else {
      m_window.unlockCursor();
    }
  }

  rayCast();
  static double lastTime = glfwGetTime();
  auto elapsed = glfwGetTime() - lastTime;
  static bool isFirstAction = true;

  if (Mouse::isPressed(GLFW_MOUSE_BUTTON_LEFT)) {
    if (elapsed < m_miningDelay) {
      blockBreakStage = std::min(static_cast<int>(elapsed / (m_miningDelay / 10)), 10);
    } else {
      m_world->setBlockWithUpdate({m_blockAimPos.x, m_blockAimPos.y, m_blockAimPos.z},
                                  Block::AIR);
      isFirstAction = false;
      blockBreakStage = 0;
      lastTime = glfwGetTime();
      m_blockAimPos = NULL_VECTOR;
    }
  } else if (Mouse::isPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
    if ((isFirstAction || elapsed >= m_placingDelay) && m_prevBlockAimPos != NULL_VECTOR) {
      m_world->setBlockWithUpdate(m_prevBlockAimPos, inventory.getHeldItem());
      isFirstAction = false;
      lastTime = glfwGetTime();
    }
  } else {
    isFirstAction = true;
    blockBreakStage = 0;
    lastTime = glfwGetTime();
  }
}

void Player::renderDebugGui() {
  ImGui::Text("Camera Position  %.2f x  %.2f y %.2f z",
              position.x, position.y, position.z);
  ImGui::Text("blockAimPos: %d, %d, %d", m_blockAimPos.x,
              m_blockAimPos.y, m_blockAimPos.z);
  ImGui::Text("prevBlockAimPos: %d, %d, %d", m_prevBlockAimPos.x,
              m_prevBlockAimPos.y, m_prevBlockAimPos.z);
  ImGui::Text("focus: %s", m_focus ? "true" : "false");

  std::string blockName = BlockDB::getBlockData(inventory.getHeldItem()).name;
  ImGui::Text("Block Type: %s", blockName.c_str());
  ImGui::Text("Block ID: %d", static_cast<int>(inventory.getHeldItem()));

  ImGui::SliderFloat("Movement Speed", &m_movementSpeed, 1.0f, 300.0f);
  ImGui::SliderInt("PlaceRadius %i", &m_placeRadius, 1, 20);
}

void Player::onCursorUpdate(double xOffset, double yOffset) {

  if (m_focus) {
    auto cursorPos = m_window.getCursorPosition();
    auto viewport = m_window.getWindowSize();
    camera.onCursorUpdate((float) ((float)viewport.x / 2.0f - cursorPos.x), (float) ((float)viewport.y / 2.0f - cursorPos.y));
    m_window.centerCursor();
    m_window.lockCursor();
  } else {
    m_window.unlockCursor();
  }

}

void Player::rayCast() {
  if (camera.getFront() == glm::vec3(0)) return;

  glm::vec3 direction = glm::normalize(camera.getFront());
  // unit direction to step x,y,z
  glm::ivec3 step = glm::sign(direction);
  glm::vec3 origin = position;
  glm::ivec3 blockPos = glm::floor(origin);

  glm::vec3 tMax =
      {intBound((float) origin.x, direction.x),
       intBound((float) origin.y, direction.y),
       intBound((float) origin.z, direction.z)};

  glm::vec3 tDelta = (glm::vec3) step / direction;
  float radius = m_rayCastRadius / glm::length(direction);

  while (true) {
    if (blockPos.z < 0 || blockPos.z >= WORLD_HEIGHT_BLOCKS) {
      m_blockAimPos = NULL_VECTOR;
      m_prevBlockAimPos = NULL_VECTOR;
      return;
    }

    // incremental, find the axis where the distance to voxel edge along that axis is the least
    if (tMax.x < tMax.y) {
      if (tMax.x < tMax.z) {
        // x < z < y
        if (tMax.x > radius) break;
        blockPos.x += step.x;
        tMax.x += tDelta.x;
      } else {
        // z < x < y
        if (tMax.z > radius) break;
        blockPos.z += step.z;
        tMax.z += tDelta.z;

      }
    } else {
      if (tMax.y < tMax.z) {
        // y < z and x
        if (tMax.y > radius) break;
        blockPos.y += step.y;
        tMax.y += tDelta.y;

      } else {
        // z < y and x
        if (tMax.z > radius) break;
        blockPos.z += step.z;
        tMax.z += tDelta.z;
      }
    }

    m_prevBlockAimPos = m_blockAimPos;
    m_blockAimPos = blockPos;
    if (m_world->getBlockFromWorldPosition(blockPos) != Block::AIR) {
      return;
    }
  }

  m_blockAimPos = NULL_VECTOR;
  m_prevBlockAimPos = NULL_VECTOR;

}


