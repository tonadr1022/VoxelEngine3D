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

    void update(float deltaTime);

    void perFrameUpdate();

    int blockBreakStage = 0;

    Inventory inventory{true};

    void processScrollInput(double yoffset);

    Camera camera;

    void renderDebugGui();

    void onCursorUpdate(double xOffset, double yOffset);

private:


    void processKeyInput(float deltaTime);

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 110.0f);

    float m_movementSpeed = 100.0f;
};


#endif //VOXEL_ENGINE_PLAYER_HPP
