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

using ChunkMap = std::unordered_map<ChunkKey, Scope<Chunk>>;


static constexpr std::array<glm::ivec2, 8> NEIGHBOR_CHUNK_KEY_OFFSETS = {
        glm::ivec2{-1, -1},
        glm::ivec2{-1, 0},
        glm::ivec2{-1, 1},
        glm::ivec2{0, -1},
        glm::ivec2{0, 1},
        glm::ivec2{1, -1},
        glm::ivec2{1, 0},
        glm::ivec2{1, 1}
};

static constexpr std::array<glm::ivec2, 9> NEIGHBOR_ARRAY_OFFSETS = {
        glm::ivec2{-1, -1},
        glm::ivec2{-1, 0},
        glm::ivec2{-1, 1},
        glm::ivec2{0, -1},
        glm::ivec2{0, 0},
        glm::ivec2{0, 1},
        glm::ivec2{1, -1},
        glm::ivec2{1, 0},
        glm::ivec2{1, 1}
};


class World {
public:
    explicit World(Renderer &renderer, int seed, const std::string &savePath);

    ~World();

    void update();

    void render();

    Player player;

    ChunkRenderer chunkRenderer;

    void renderDebugGui();

private:
    Block getBlockFromWorldPosition(glm::ivec3 position);

    void setBlockFromWorldPosition(glm::ivec3 position, Block block);

    static inline ChunkKey getChunkKeyByWorldLocation(int x, int y) {
        return ChunkKey{static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_WIDTH)),
                        static_cast<int>(std::floor(static_cast<float>(y) / CHUNK_WIDTH))};
    }

    static inline ChunkKey getChunkKeyByWorldLocation(const glm::ivec3 &position) {
        return getChunkKeyByWorldLocation(position.x, position.y);
    }

    inline Chunk *getChunkRawPtr(ChunkKey chunkKey) {
        return m_chunkMap[chunkKey].get();
    }

    ChunkMap m_chunkMap;

    inline bool chunkExists(ChunkKey chunkKey) {
        return m_chunkMap.find(chunkKey) != m_chunkMap.end();
    }

    bool hasAllNeighbors(ChunkKey chunkKey);

    bool hasAllNeighborsFullyGenerated(ChunkKey chunkKey);

    void castPlayerAimRay(Ray ray);

    void saveData();

    glm::ivec3 m_lastRayCastBlockPos = NULL_VECTOR;
    glm::ivec3 m_prevLastRayCastBlockPos = NULL_VECTOR;

    void loadChunks();

    void loadChunksWorker();

    void unloadChunks();

    void updateChunkMeshes();

    void updateChunkLoadList();

    void updateChunkMeshList();


    std::vector<ChunkKey> m_chunksToUnload;


    std::mutex m_mainMutex;
    std::atomic<bool> m_isRunning{true};
    std::condition_variable m_conditionVariable;

    std::vector<std::thread> m_chunkLoadThreads;
    std::atomic_uint m_numRunningThreads;


    int m_renderDistance = 16;

    Renderer m_renderer;

    ChunkManager m_chunkManager;

    TerrainGenerator m_terrainGenerator;

    WorldSave m_worldSave;

    std::vector<ChunkKey> m_chunksInMeshRangeVector;
    std::unordered_map<ChunkKey, std::unique_ptr<ChunkMeshInfo>> m_chunkMeshInfoMap;
    std::list<ChunkKey> m_chunksReadyToMeshList;

    std::unordered_set<ChunkKey> m_renderSet;

    std::vector<ChunkKey> m_chunksToLoadVector;
    std::unordered_map<ChunkKey, std::unique_ptr<ChunkLoadInfo>> m_chunkTerrainLoadInfoMap;

    std::vector<ChunkKey> m_chunksToGenerateStructuresVector;
    std::unordered_map<ChunkKey, std::unique_ptr<ChunkGenerateStructuresInfo>> m_chunkGenerateStructuresInfoMap;

    unsigned int m_numLoadingThreads;

    inline bool cmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - (glm::vec2)m_center) <
               glm::length(glm::vec2(r) - (glm::vec2)m_center);
    }

    inline bool rcmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
        return glm::length(glm::vec2(l) - (glm::vec2)m_center) >
               glm::length(glm::vec2(r) - (glm::vec2)m_center);
    }

    inline bool rcmpChunkKey_impl(const ChunkKey &l, const ChunkKey &r) const {
        return glm::length(glm::vec2(l.x, l.y) - (glm::vec2)m_center) >
               glm::length(glm::vec2(r.x, r.y) - (glm::vec2)m_center);
    }

    inline bool cmpChunkKey_impl(const ChunkKey &l, const ChunkKey &r) const {
        return glm::length(glm::vec2(l.x, l.y) - (glm::vec2)m_center) <
               glm::length(glm::vec2(r.x, r.y) - (glm::vec2)m_center);
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

    glm::ivec2 m_center;

    bool m_xyChanged = false;

    int m_seed;

};


#endif //VOXEL_ENGINE_WORLD_HPP
