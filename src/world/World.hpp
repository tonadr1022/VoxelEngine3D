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

    void loadChunksWorker();

    void unloadChunks();

    void updateChunkMeshes();

    void updateChunkLoadList();


    std::vector<ChunkKey> m_chunksToUnload;


    std::mutex m_mainMutex;
    std::atomic<bool> m_isRunning{true};
    std::condition_variable m_conditionVariable;

    std::vector<std::thread> m_chunkLoadThreads;
    std::vector<std::thread> m_chunkMeshThreads;
    std::atomic_uint m_numRunningThreads;


    int m_renderDistance = 12;

    Renderer m_renderer;

    ChunkManager m_chunkManager;

    TerrainGenerator m_terrainGenerator;

    WorldSave m_worldSave;

    std::vector<ChunkKey> m_chunksInMeshRangeVector;
    std::unordered_map<ChunkKey, std::unique_ptr<ChunkMeshInfo>> m_chunkMeshInfoMap;
    std::list<ChunkKey> m_chunksReadyToMeshList;

    std::unordered_set<ChunkKey> m_renderSet;

    std::vector<ChunkKey> m_chunksToLoadVector;
    std::unordered_map<ChunkKey, std::unique_ptr<ChunkLoadInfo>> m_chunkLoadInfoMap;

    unsigned int m_numLoadingThreads;

    inline bool cmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - glm::vec2(m_center.x, m_center.y)) <
               glm::length(glm::vec2(r) - glm::vec2(m_center.x, m_center.y));
    }

    inline bool rcmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - glm::vec2(m_center.x, m_center.y)) >
               glm::length(glm::vec2(r) - glm::vec2(m_center.x, m_center.y));
    }

    inline bool cmpVec3_impl(const glm::ivec3 &l, const glm::ivec3 &r) {
        return glm::length(glm::vec3(l) - (glm::vec3) m_center) <
               glm::length(glm::vec3(r) - (glm::vec3) m_center);
    }

    inline bool rcmpVec3_impl(const glm::ivec3 &l, const glm::ivec3 &r) {
        return glm::length(glm::vec3(l) - (glm::vec3) m_center) >
               glm::length(glm::vec3(r) - (glm::vec3) m_center);
    }

    inline bool rcmpChunkKey_impl(const ChunkKey &l, const ChunkKey &r) const {
        return glm::length(glm::vec2(l.x, l.y) - glm::vec2(m_center.x, m_center.y)) >
               glm::length(glm::vec2(r.x, r.y) - glm::vec2(m_center.x, m_center.y));
    }

    inline bool cmpChunkKey_impl(const ChunkKey &l, const ChunkKey &r) const {
        return glm::length(glm::vec2(l.x, l.y) - glm::vec2(m_center.x, m_center.y)) <
               glm::length(glm::vec2(r.x, r.y) - glm::vec2(m_center.x, m_center.y));
    }

    std::function<bool(const ChunkKey &, const ChunkKey &)> cmpChunkKey = [this](auto &&PH1,
                                                                                 auto &&PH2) {
        return cmpChunkKey_impl(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };

    std::function<bool(const ChunkKey &, const ChunkKey &)> rcmpChunkKey;

    std::function<bool(const glm::ivec2 &, const glm::ivec2 &)> cmpVec2 = [this](auto &&PH1,
                                                                                 auto &&PH2) {
        return cmpVec2_impl(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    std::function<bool(const glm::ivec2 &, const glm::ivec2 &)> rcmpVec2 = [this](auto &&PH1,
                                                                                  auto &&PH2) {
        return rcmpVec2_impl(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    std::function<bool(const glm::ivec3 &, const glm::ivec3 &)> cmpVec3 = [this](auto &&PH1,
                                                                                 auto &&PH2) {
        return cmpVec3_impl(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };
    std::function<bool(const glm::ivec3 &, const glm::ivec3 &)> rcmpVec3 = [this](auto &&PH1,
                                                                                  auto &&PH2) {
        return rcmpVec3_impl(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    };

    glm::ivec3 m_center;

    bool m_xyChanged = false;

};


#endif //VOXEL_ENGINE_WORLD_HPP
