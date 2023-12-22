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
#include "../EngineConfig.hpp"
#include "generation/TerrainGenerator.hpp"
#include "save/WorldSave.hpp"
#include <glm/gtx/hash.hpp>

class World {
public:
    explicit World(Renderer &renderer, int seed, const std::string &savePath);

    ~World();

    void update();

    void castPlayerAimRay(Ray ray);

    void render();

    Player player;

    ChunkRenderer chunkRenderer;

    int getRenderDistance() const;

    void setRenderDistance(int renderDistance);

    void initialize(const std::function<void()> &callback);

    void renderDebugGui();

private:
    void saveData();

    glm::ivec3 m_lastRayCastBlockPos = NULL_VECTOR;
    glm::ivec3 m_prevLastRayCastBlockPos = NULL_VECTOR;

    void loadChunks();

    void unloadChunks();

    void updateChunks();

    void updateChunkMeshes();


    std::vector<ChunkKey> m_chunksToUnload;

    std::vector<glm::ivec3> m_chunksToMeshVector;

    std::mutex m_mainMutex;
    std::atomic<bool> m_isRunning{true};
    bool m_isInitializing = false;

    std::vector<std::thread> m_chunkLoadThreads;
    std::vector<std::thread> m_chunkMeshThreads;

    int m_renderDistance = 12;

    Renderer m_renderer;

    ChunkManager m_chunkManager;

    TerrainGenerator m_terrainGenerator;

    WorldSave m_worldSave;

    std::unordered_map<glm::ivec2, std::unique_ptr<ChunkLoadInfo>> m_loadInfoMap;

    inline bool cmpVec2(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - glm::vec2(m_center.x, m_center.y)) <
               glm::length(glm::vec2(r) - glm::vec2(m_center.x, m_center.y));
    }

    inline bool rcmpVec2(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - glm::vec2(m_center.x, m_center.y)) >
               glm::length(glm::vec2(r) - glm::vec2(m_center.x, m_center.y));
    }

    inline bool cmpVec3(const glm::ivec3 &l, const glm::ivec3 &r) {
        return glm::length(glm::vec3(l) - (glm::vec3) m_center) <
               glm::length(glm::vec3(r) - (glm::vec3) m_center);
    }

    inline bool rcmpVec3(const glm::ivec3 &l, const glm::ivec3 &r) {
        return glm::length(glm::vec3(l) - (glm::vec3) m_center) >
               glm::length(glm::vec3(r) - (glm::vec3) m_center);
    }

    glm::ivec3 m_center;

};


#endif //VOXEL_ENGINE_WORLD_HPP
