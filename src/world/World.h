//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_WORLD_H
#define VOXEL_ENGINE_WORLD_H

#include "../player/Player.h"
#include "../camera/Camera.h"
#include "generation/TerrainGenerator.h"
#include "chunk/ChunkRenderer.h"
#include "chunk/ChunkManager.h"
#include "../physics/Ray.h"
#include "events/IEvent.h"
#include "../renderer/Renderer.h"
#include <thread>


class World {
public:
    World(GLFWwindow *window, Renderer &renderer);

    ~World();

    void update();

    void castRay(Ray ray);

    void render();

    int renderDistance = 8;

    void addEvent(std::unique_ptr<IEvent> event);

    Player player;

private:
    GLFWwindow *window;
    glm::ivec3 lastRayCastBlockPos = glm::ivec3(-1, -1, -1);
    std::vector<std::unique_ptr<IEvent>> events;

    void loadChunks();

    void unloadChunks();

    void updateChunksToUnload();

    void updateChunks();

    void setBlock(glm::ivec3 position, Block block);

    void updateChunkMeshes(ChunkKey &playerChunkKeyPos);

    void reloadChunksToReload();

    void handleChunkUpdates(Chunk& chunk, ChunkKey chunkKey, int chunkX, int chunkY);

    std::vector<ChunkKey> chunksToReload;
    std::vector<ChunkKey> m_chunksToUnload;

    std::mutex m_mainMutex;
    std::atomic<bool> m_isRunning{true};

    std::vector<std::thread> m_chunkLoadThreads;

    ChunkRenderer chunkRenderer;
    ChunkManager chunkManager;
    Renderer renderer;
};


#endif //VOXEL_ENGINE_WORLD_H
