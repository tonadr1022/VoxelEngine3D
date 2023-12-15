//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_WORLD_HPP
#define VOXEL_ENGINE_WORLD_HPP

#include "../renderer/Renderer.hpp"
#include "chunk/ChunkRenderer.hpp"
#include "../player/Player.hpp"
#include "chunk/ChunkManager.hpp"
#include "../physics/Ray.hpp"

#include <thread>



class World {
public:
    World(GLFWwindow *window, Renderer &renderer);

    ~World();

    void update();

    void castPlayerAimRay(Ray ray);

    void render();

//    void addEvent(std::unique_ptr<IEvent> event);

    Player player;

    ChunkRenderer chunkRenderer;

    int getRenderDistance() const;

    void setRenderDistance(int renderDistance);

private:
    GLFWwindow *window;
    glm::ivec3 lastRayCastBlockPos = NULL_VECTOR;
    glm::ivec3 prevLastRayCastBlockPos = NULL_VECTOR;
//    std::vector<std::unique_ptr<IEvent>> events;

    void loadChunks();

    void unloadChunks();

    void updateChunks();

    void setBlock(glm::ivec3 position, Block block);

    void updateChunkMeshes();

    void reloadChunksToReload();

    void handleChunkUpdates(Chunk& chunk, ChunkKey chunkKey, int chunkX, int chunkY);

    std::vector<ChunkKey> chunksToReload;
    std::vector<ChunkKey> m_chunksToUnload;

    std::mutex m_mainMutex;
    std::atomic<bool> m_isRunning{true};

    std::vector<std::thread> m_chunkLoadThreads;
    std::vector<std::thread> m_chunkMeshThreads;

    int m_renderDistance = 12;
    int m_loadDistanceChunks = m_renderDistance + 1;


    Renderer renderer;

    ChunkManager chunkManager;

};


#endif //VOXEL_ENGINE_WORLD_HPP
