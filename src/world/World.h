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
#include "../physics/Ray.h"
#include "events/IEvent.h"


class World {
public:
    World(GLFWwindow *window, Player &player);

    void update();

    void castRay(Ray ray);

    void render();

    int renderDistance = 8;

    void addEvent(std::unique_ptr<IEvent> event);

private:
    GLFWwindow *window;
    int chunksToLoadPerFrame = 1;
    int chunksLoadedThisFrame = 0;
    glm::ivec3 lastRayCastBlockPos = glm::ivec3(0, 0, 0);
    std::vector<std::unique_ptr<IEvent>> events;

    void loadChunks(ChunkKey &playerChunkKeyPos, bool shouldLoadAll = false);
//    void updateChunkMeshes(ChunkKey &playerChunkKeyPos, bool shouldUpdateAll = false);

    void unloadChunks();
    Player &player;
    ChunkRenderer chunkRenderer;
    ChunkManager chunkManager;
};


#endif //VOXEL_ENGINE_WORLD_H
