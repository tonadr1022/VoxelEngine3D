//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_WORLD_HPP
#define VOXEL_ENGINE_WORLD_HPP

#include <glm/gtx/hash.hpp>

#include "../EngineConfig.hpp"
#include "../physics/Ray.hpp"
#include "../player/Player.hpp"
#include "../renderer/Renderer.hpp"
#include "generation/TerrainGenerator.hpp"
#include "save/WorldSave.hpp"

using ChunkMap = std::unordered_map<glm::ivec2, Scope<Chunk>>;

static constexpr std::array<glm::ivec2, 8> NEIGHBOR_CHUNK_KEY_OFFSETS = {
    glm::ivec2{-1, -1}, glm::ivec2{-1, 0}, glm::ivec2{-1, 1}, glm::ivec2{0, -1},
    glm::ivec2{0, 1}, glm::ivec2{1, -1}, glm::ivec2{1, 0}, glm::ivec2{1, 1}};

static constexpr std::array<glm::ivec2, 9> NEIGHBOR_ARRAY_OFFSETS = {
    glm::ivec2{-1, -1}, glm::ivec2{-1, 0}, glm::ivec2{-1, 1},
    glm::ivec2{0, -1}, glm::ivec2{0, 0}, glm::ivec2{0, 1},
    glm::ivec2{1, -1}, glm::ivec2{1, 0}, glm::ivec2{1, 1}};

class World {
 public:
  explicit World(Renderer &renderer, int seed, const std::string &savePath);
  ~World();

  void update();
  void renderDebugGui();
  inline const std::unordered_set<glm::ivec2> &getOpaqueRenderSet() const { return m_opaqueRenderSet; }
  inline const std::vector<glm::ivec2> &getTransparentRenderVector() const { return m_transparentRenderVector; }
  Player player;

  inline Chunk *getChunkRawPtr(const glm::ivec2 &pos) const {
    return m_chunkMap.at(pos).get();
  }

  inline Chunk *getChunkRawPtrOrNull(const glm::ivec2 &pos) const {
    if (!chunkExists(pos)) return nullptr;
    return m_chunkMap.at(pos).get();
  }
  inline const glm::ivec3 &getLastRayCastBlockPos() const { return m_lastRayCastBlockPos; }

 private:
  Block getBlockFromWorldPosition(glm::ivec3 position) const;
  void setRenderDistance(int renderDistance);

  static inline glm::ivec2 chunkPosFromWorldPos(int x, int y) {
    return glm::ivec2{static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_WIDTH)),
                      static_cast<int>(std::floor(static_cast<float>(y) / CHUNK_WIDTH))};
  }

  static inline glm::ivec2 chunkPosFromWorldPos(glm::ivec2 pos) {
    return glm::ivec2{static_cast<int>(
                          std::floor(static_cast<float>(pos.x) / CHUNK_WIDTH)),
                      static_cast<int>(std::floor(static_cast<float>(pos.y) / CHUNK_WIDTH))};
  }

  inline bool chunkExists(const glm::ivec2 &pos) const {
    return m_chunkMap.find(pos) != m_chunkMap.end();
  }

  bool hasAllNeighborsInState(const glm::ivec2 &pos, ChunkState state);
  bool hasAllNeighborsInStates(const glm::ivec2 &pos, ChunkState state1, ChunkState state2);
  void castPlayerAimRay(Ray ray);
  void setBlockWithUpdate(const glm::ivec3 &worldPos, Block block);
  void setBlockWithUpdate(int worldX, int worldY, int worldZ, Block block);
  void saveData();

  void unloadChunks();

  void updateChunkLoadList();
  void updateChunkStructureGenList();
  void updateChunkMeshList();
//  void updateChunkUpdateList();
  void processDirectChunkUpdates();

  void generateTerrainWorker();
  void generateStructuresWorker();
  void generateChunkMeshWorker();
  void generateChunksWorker();
  void meshUpdateWorker();

  float m_frameTime = 0;
  int m_renderDistance = 16;
  int m_loadDistance = m_renderDistance + 2;
  int m_structureLoadDistance = m_renderDistance + 1;
  int m_unloadDistance = m_renderDistance + 6;

  glm::ivec2 m_center;
  bool m_xyChanged = false;
  bool m_renderDistanceChanged = false;
  int m_seed;
  glm::ivec3 m_lastRayCastBlockPos = NULL_VECTOR;
  glm::ivec3 m_prevLastRayCastBlockPos = NULL_VECTOR;

  ChunkMap m_chunkMap;
  Renderer m_renderer;
  WorldSave m_worldSave;

  std::mutex m_mainMutex;
  std::atomic<bool> m_isRunning{true};
  std::condition_variable m_conditionVariable;
  std::vector<std::thread> m_chunkLoadThreads;
  std::atomic_uint m_numRunningThreads;
  unsigned int m_numLoadingThreads;

  std::vector<glm::ivec2> m_chunksToLoadVector;
  std::unordered_map<glm::ivec2, Scope<ChunkLoadInfo>>
      m_chunkTerrainLoadInfoMap;
  std::unordered_map<glm::ivec2, Scope<std::array<int, CHUNK_AREA>>> m_heightMapsMap;

  std::vector<glm::ivec2> m_chunksInStructureGenRangeVector;
  std::unordered_map<glm::ivec2, Scope<ChunkGenerateStructuresInfo>> m_chunkStructureGenInfoMap;
  std::list<glm::ivec2> m_chunksReadyToGenStructuresList;

  std::vector<glm::ivec2> m_chunksInMeshRangeVector;
  std::unordered_map<glm::ivec2, Scope<ChunkMeshInfo>>
      m_chunkMeshInfoMap;
  std::list<glm::ivec2> m_chunksReadyToMeshList;

  std::vector<glm::ivec2> m_chunkUpdateVector;
  std::unordered_map<glm::ivec2, Scope<ChunkMeshInfo>> m_chunkUpdateInfoMap;

  std::unordered_set<glm::ivec2> m_chunkDirectlyUpdateSet;

  std::unordered_set<glm::ivec2> m_opaqueRenderSet;
  std::unordered_set<glm::ivec2> m_transparentRenderSet;
  std::vector<glm::ivec2> m_transparentRenderVector;

  void sortTransparentRenderVector();

  inline bool cmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
    return glm::length(glm::vec2(l) - (glm::vec2) m_center) <
        glm::length(glm::vec2(r) - (glm::vec2) m_center);
  }

  inline bool rcmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
    return glm::length(glm::vec2(l) - (glm::vec2) m_center) >
        glm::length(glm::vec2(r) - (glm::vec2) m_center);
  }

  std::function<bool(const glm::ivec2 &, const glm::ivec2 &)> cmpVec2 =
      [this](auto &&PH1, auto &&PH2) {
        return cmpVec2_impl(std::forward<decltype(PH1)>(PH1),
                            std::forward<decltype(PH2)>(PH2));
      };
  std::function<bool(const glm::ivec2 &, const glm::ivec2 &)> rcmpVec2 =
      [this](auto &&PH1, auto &&PH2) {
        return rcmpVec2_impl(std::forward<decltype(PH1)>(PH1),
                             std::forward<decltype(PH2)>(PH2));
      };
};

#endif  // VOXEL_ENGINE_WORLD_HPP
