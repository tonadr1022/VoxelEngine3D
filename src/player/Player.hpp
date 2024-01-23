//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_PLAYER_HPP
#define VOXEL_ENGINE_PLAYER_HPP

#include "../camera/Camera.hpp"
#include "Inventory.hpp"
#include "../EngineConfig.hpp"

class Player {
 public:
  Player();

  glm::vec3 &getPosition();
  [[nodiscard]] inline glm::ivec3 getChunkPosition() const { return {static_cast<int>(position.x / CHUNK_SIZE),
                                                 static_cast<int>(position.y / CHUNK_SIZE),
                                                 static_cast<int>(position.z / CHUNK_SIZE)};}
  void update(float deltaTime);
  void perFrameUpdate();
  void processScrollInput(double yoffset);
  void renderDebugGui();
  void onCursorUpdate(double xOffset, double yOffset);

  Inventory inventory{true};
  Camera camera;
  int blockBreakStage = 0;

 private:

  void processKeyInput(float deltaTime);

  glm::vec3 position = glm::vec3(0.0f, 0.0f, 32.0f);

  float m_movementSpeed = 10.0f;
};

#endif //VOXEL_ENGINE_PLAYER_HPP
