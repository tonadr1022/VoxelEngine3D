//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_PLAYER_HPP
#define VOXEL_ENGINE_PLAYER_HPP

#include "../camera/Camera.hpp"
#include "Inventory.hpp"
#include "../EngineConfig.hpp"

class World;
class Window;
class Player {
 public:
  Player(World *world, Window &window);

  glm::vec3 &getPosition();
  [[nodiscard]] inline glm::ivec3 getChunkPosition() const { return {static_cast<int>(position.x / CHUNK_SIZE),
                                                 static_cast<int>(position.y / CHUNK_SIZE),
                                                 static_cast<int>(position.z / CHUNK_SIZE)};}
  void update(double deltaTime);
  void perFrameUpdate();
  void processScrollInput(double yoffset);
  void renderDebugGui();
  void onCursorUpdate(double xOffset, double yOffset);

  void rayCast();

  inline float intBound(float s, float ds)
  {
    bool sIsInteger = glm::round(s) == s;
    if (ds < 0 && sIsInteger)
      return 0;
    return (ds > 0 ? (s == 0.0f ? 1.0f : glm::ceil(s)) - s : s - glm::floor(s)) / glm::abs(ds);
  }

  Inventory inventory{true};
  Camera camera;
  int blockBreakStage = 0;
  glm::ivec3 m_blockAimPos = NULL_VECTOR;
  glm::ivec3 m_prevBlockAimPos = NULL_VECTOR;

  void setFocus(bool focus);

 private:
  World *m_world;
  Window &m_window;

  void processKeyInput(double deltaTime);

  bool m_focus = true;
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 100.0f);
  float m_miningDelay = 0.3f;
  float m_placingDelay = 0.1f;
  float m_rayCastRadius = 10.0f;
  float m_movementSpeed = 10.0f;
};

#endif //VOXEL_ENGINE_PLAYER_HPP
