//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_PLAYER_H
#define VOXEL_ENGINE_PLAYER_H

#include "../Config.h"
#include "../camera/Camera.h"
#include "../world/chunk/ChunkKey.h"
#include "Inventory.h"


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
private:


    void processKeyInput(GLFWwindow *window, float deltaTime);
    void processMouseInput(GLFWwindow *window, float deltaTime);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 70.0f);

    float movementSpeed = 10.0f;




};


#endif //VOXEL_ENGINE_PLAYER_H
