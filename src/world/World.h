//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_WORLD_H
#define VOXEL_ENGINE_WORLD_H

#include "../Config.h"
#include "../player/Player.h"
#include "../camera/Camera.h"
#include "generation/TerrainGenerator.h"
#include "chunk/ChunkRenderer.h"
#include "chunk/ChunkManager.h"


class World {
public:
    World(GLFWwindow *window, Player &player, Shader &chunkShader);

    void update();

    void render();

private:
    std::deque<Chunk> chunkLoadQueue;
    void unloadChunks();
    GLFWwindow *window;
    Player &player;
    ChunkRenderer chunkRenderer;
    ChunkManager chunkManager;


};


#endif //VOXEL_ENGINE_WORLD_H
