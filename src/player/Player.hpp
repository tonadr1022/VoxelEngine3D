//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_PLAYER_HPP
#define VOXEL_ENGINE_PLAYER_HPP

#include "../camera/Camera.hpp"
#include "../world/chunk/ChunkKey.hpp"
#include "Inventory.hpp"
#include "../EngineConfig.hpp"

class Player {
public:
    Player();

    glm::vec3 &getPosition();

    [[nodiscard]] ChunkKey getChunkKeyPos() const;

    void update(GLFWwindow *window, float deltaTime);

    void perFrameUpdate(GLFWwindow *window);

    int blockBreakStage = 0;

    Inventory inventory{true};

    void processScrollInput(double yoffset);

    Camera camera;

    [[nodiscard]] float getMovementSpeed() const;

    void setMovementSpeed(float movementSpeed);

private:


    void processKeyInput(GLFWwindow *window, float deltaTime);

    void processMouseInput(GLFWwindow *window, float deltaTime);

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 70.0f);

    float m_movementSpeed = 70.0f;
};


#endif //VOXEL_ENGINE_PLAYER_HPP
