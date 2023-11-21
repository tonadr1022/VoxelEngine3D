//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_PLAYER_H
#define VOXEL_ENGINE_PLAYER_H

#include "../Config.h"
#include "../camera/Camera.h"

class Player {
public:
    Player();

    glm::vec3 &getPosition();
    [[nodiscard]] std::pair<int, int> getChunkKeyPos() const;


//    void processKeyInput(int key, int scancode, int action, int mods);

    void update(GLFWwindow *window, float deltaTime);

    Camera camera;
private:
    void processKeyInput(GLFWwindow *window, float deltaTime);
    void processMouseInput(GLFWwindow *window, float deltaTime);
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);

    float movementSpeed = 20.0f;


};


#endif //VOXEL_ENGINE_PLAYER_H
