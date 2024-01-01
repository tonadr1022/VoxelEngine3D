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

using ChunkMap = std::unordered_map<glm::ivec3, Scope<Chunk>>;

//static constexpr std::array<glm::ivec2, 8> NEIGHBOR_CHUNK_KEY_OFFSETS = {
//    glm::ivec2{-1, -1}, glm::ivec2{-1, 0}, glm::ivec2{-1, 1}, glm::ivec2{0, -1},
//    glm::ivec2{0, 1}, glm::ivec2{1, -1}, glm::ivec2{1, 0}, glm::ivec2{1, 1}};

// z
// |
// |  6   15  24
// |    7   16  25
// |      8   17  26
// |
// |  3   12  21
// |    4   13  22
// |      5   14  23
// \-------------------y
//  \ 0   9   18
//   \  1   10  19
//    \   2   11  20
//     x
static constexpr std::array<glm::ivec3, 27> NEIGHBOR_ARRAY_OFFSETS =
    {
        // z == -1
        glm::ivec3{-1, -1, -1}, glm::ivec3{0, -1, -1}, glm::ivec3{1, -1, -1},
        glm::ivec3{-1, 0, -1}, glm::ivec3{0, 0, -1}, glm::ivec3{1, 0, -1},
        glm::ivec3{-1, 1, -1}, glm::ivec3{0, 1, -1}, glm::ivec3{1, 1, -1},

        // z == 0
        glm::ivec3{-1, -1, 0}, glm::ivec3{0, -1, 0}, glm::ivec3{1, -1, 0},
        glm::ivec3{-1, 0, 0}, glm::ivec3{0, 0, 0}, glm::ivec3{1, 0, 0},
        glm::ivec3{-1, 1, 0}, glm::ivec3{0, 1, 0}, glm::ivec3{1, 1, 0},

        // z == 1
        glm::ivec3{-1, -1, 1}, glm::ivec3{0, -1, 1}, glm::ivec3{1, -1, 1},
        glm::ivec3{-1, 0, 1}, glm::ivec3{0, 0, 1}, glm::ivec3{1, 0, 1},
        glm::ivec3{-1, 1, 1}, glm::ivec3{0, 1, 1}, glm::ivec3{1, 1, 1}
    };


//static constexpr std::array<glm::ivec3, 6> DIRECTLY_ADJACENT_CHUNK_OFFSETS = {
//    glm::ivec3{0, 0, 1},  // top
//    glm::ivec3{0, 0, -1}, // bottom
//    glm::ivec3{1, 0, 0}, // front (pos X)
//    glm::ivec3{-1, 0, 0}, // back (neg X)
//    glm::ivec3{0, 1, 0}, // right (pos Y)
//    glm::ivec3{0, -1, 0}, // left (neg y)
//};

class World {
 public:
  explicit World(Renderer &renderer, int seed, const std::string &savePath);
  ~World();

  void update();
  void renderDebugGui();
  inline const std::unordered_set<glm::ivec3> &getOpaqueRenderSet() const { return m_opaqueRenderSet; }
  inline const std::vector<glm::ivec3> &getTransparentRenderVector() const { return m_transparentRenderVector; }
  Player player;

  inline Chunk *getChunkRawPtr(const glm::ivec3 &pos) const {
    return m_chunkMap.at(pos).get();
  }

  inline Chunk *getChunkRawPtrOrNull(const glm::ivec3 &pos) const {
    if (!chunkExists(pos)) return nullptr;
    return m_chunkMap.at(pos).get();
  }
  inline const glm::ivec3 &getLastRayCastBlockPos() const { return m_lastRayCastBlockPos; }

 private:
  Block getBlockFromWorldPosition(const glm::ivec3 &position) const;

  void setRenderDistance(int renderDistance);


  static inline glm::ivec3 chunkPosFromWorldPos(int x, int y, int z) {
    return glm::ivec3{static_cast<int>(std::floor(static_cast<float>(x) / CHUNK_SIZE)),
                      static_cast<int>(std::floor(static_cast<float>(y) / CHUNK_SIZE)),
                      static_cast<int>(std::floor(static_cast<float>(z) / CHUNK_SIZE))};
  }

  static inline glm::ivec3 chunkPosFromWorldPos(glm::ivec3 pos) {
    return glm::ivec3{static_cast<int>(std::floor(static_cast<float>(pos.x) / CHUNK_SIZE)),
                      static_cast<int>(std::floor(static_cast<float>(pos.y) / CHUNK_SIZE)),
                      static_cast<int>(std::floor(static_cast<float>(pos.z) / CHUNK_SIZE))};
  }

  inline bool chunkExists(const glm::ivec3 &pos) const {
    return m_chunkMap.find(pos) != m_chunkMap.end();
  }

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
  void addNeighborChunks(Chunk *(&chunks)[27], const glm::ivec3 &pos) const;

  void generateChunksWorker4();
  void processBatchToLoad(std::queue<glm::ivec2> &batchToLoad);
  void processBatchToGenStructures(std::queue<glm::ivec3> &batchToGenStructures);
  void processBatchToMesh(std::queue<glm::ivec3> &batchToMesh);

//  void meshUpdateWorker();

  int m_renderDistance = 8;
  int m_loadDistance = m_renderDistance + 2;
  int m_structureLoadDistance = m_renderDistance + 1;
  int m_unloadDistance = m_renderDistance + 4;

  glm::ivec3 m_center;
  glm::ivec2 m_xyCenter;
  bool m_centerChanged = false;
  bool m_centerChangedXY = false;
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
  static constexpr int MAX_BATCH_SIZE = 30;

  std::vector<glm::ivec2> m_chunksToLoadVector;
  std::unordered_map<glm::ivec2, Scope<ChunkTerrainInfo>> m_chunkTerrainLoadInfoMap;

  std::vector<glm::ivec2> m_chunksInStructureGenRangeVectorXY;
  std::unordered_map<glm::ivec3, Scope<ChunkStructuresInfo>> m_chunkStructuresInfoMap;
  std::list<glm::ivec3> m_chunksReadyToGenStructuresList;

  std::vector<glm::ivec3> m_chunksInMeshRangeVector;
  std::unordered_map<glm::ivec3, Scope<ChunkMeshInfo>> m_chunkMeshInfoMap;
  std::list<glm::ivec3> m_chunksReadyToMeshList;

  std::vector<glm::ivec3> m_chunkUpdateVector;
  std::unordered_map<glm::ivec3, Scope<ChunkMeshInfo>> m_chunkUpdateInfoMap;

  std::unordered_set<glm::ivec3> m_chunkDirectlyUpdateSet;

  std::unordered_set<glm::ivec3> m_opaqueRenderSet;
  std::unordered_set<glm::ivec3> m_transparentRenderSet;
  std::vector<glm::ivec3> m_transparentRenderVector;

  void sortTransparentRenderVector();

  inline bool cmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
    return glm::length(glm::vec2(l) - (glm::vec2) m_center) <
        glm::length(glm::vec2(r) - (glm::vec2) m_center);
  }

  inline bool rcmpVec2_impl(const glm::ivec2 &l, const glm::ivec2 &r) const {
    return glm::length(glm::vec2(l) - (glm::vec2) m_center) >
        glm::length(glm::vec2(r) - (glm::vec2) m_center);
  }

  inline bool rcmpVec3_impl(const glm::ivec3 &l, const glm::ivec3 &r) const {
    return glm::length(glm::vec3(l) - (glm::vec3) m_center) >
        glm::length(glm::vec3(r) - (glm::vec3) m_center);
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

  std::function<bool(const glm::ivec3 &, const glm::ivec3 &)> rcmpVec3 =
      [this](auto &&PH1, auto &&PH2) {
        return rcmpVec3_impl(std::forward<decltype(PH1)>(PH1),
                             std::forward<decltype(PH2)>(PH2));
      };
};

#endif  // VOXEL_ENGINE_WORLD_HPP
