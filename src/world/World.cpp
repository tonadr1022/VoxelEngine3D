//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.hpp"
#include "../Config.hpp"
#include "block/BlockDB.hpp"
#include "../input/Mouse.hpp"
#include "../utils/Timer.hpp"
#include "chunk/ChunkAlg.hpp"

World::World(Renderer &renderer, int seed, const std::string &savePath)
    : m_worldSave(savePath), m_renderer(renderer), m_center(INT_MAX), m_xyCenter(INT_MAX), m_numRunningThreads(0),
      m_numLoadingThreads(std::thread::hardware_concurrency()), m_seed(seed), m_terrainGenerator(seed) {
//  m_numLoadingThreads = 1;
  const size_t loadVectorSize = ((size_t) (m_renderDistance + 2) * 2 + 1) * ((size_t) (m_renderDistance + 2) * 2 + 1);
  m_chunksToLoadVector.reserve(loadVectorSize);
  BlockDB::loadData("resources/blocks/");

  for (unsigned int i = 0; i < m_numLoadingThreads; i++) {
    m_chunkLoadThreads.emplace_back(&World::generateChunksWorker4, this);
  }
}

World::~World() {
  m_isRunning = false;
  m_conditionVariable.notify_all();
  for (auto &thread : m_chunkLoadThreads) {
    thread.join();
  }
  saveData();
}

void World::update() {
  Timer t("update", false);
  m_centerChanged = false;
  m_centerChangedXY = false;

  auto playerChunkPos = player.getChunkPosition();
  if (playerChunkPos != m_center) {
    m_centerChanged = true;
    if (playerChunkPos.x != m_center.x || playerChunkPos.y != m_center.y) {
      m_centerChangedXY = true;
      m_xyCenter = {playerChunkPos.x, playerChunkPos.y};
    }
    m_center = playerChunkPos;

  }

  if (m_centerChangedXY || m_renderDistanceChanged) {
    // create chunks in range (not terrain generated at this point)
    glm::ivec3 pos;
    for (pos.x = m_center.x - m_loadDistance; pos.x <= m_center.x + m_loadDistance; pos.x++) {
      for (pos.y = m_center.y - m_loadDistance; pos.y <= m_center.y + m_loadDistance; pos.y++) {
        for (pos.z = 0; pos.z < CHUNKS_PER_STACK; pos.z++) {
          if (chunkExists(pos)) continue; // skip existent chunks
          m_chunkMap.emplace(pos, std::make_unique<Chunk>(pos));
        }
      }
    }

    unloadChunks();
    m_renderDistanceChanged = false;
  }

  castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});


  // don't need to sort transparent render vector every frame.
  static unsigned long oldRenderSetSize = 0;
  unsigned long newRenderSetSize = m_transparentRenderSet.size();
  if (newRenderSetSize != oldRenderSetSize) {
    sortTransparentRenderVector();
    oldRenderSetSize = newRenderSetSize;
  }

//  static int i = 0;
//  if (i % 4 == 0) {
//    sortTransparentRenderVector();
//    i = 0;
//  }
//  i++;

  processDirectChunkUpdates();

  std::lock_guard<std::mutex> lock(m_mainMutex);
//  combinedUpdateMapsTEST();
//  combinedUpdateListsTEST();
//  combinedUpdateSortedTEST();
  updateChunkLoadList();
  updateChunkStructureGenList();
  updateChunkLightingList();
  updateChunkMeshList();
  m_conditionVariable.notify_all();
}

void World::combinedUpdateMapsTEST() {
  // apply terrain and update map regardless of whether pos changed
  for (auto posIt = m_chunkTerrainLoadInfoMap.begin(); posIt != m_chunkTerrainLoadInfoMap.end();) {
    if (posIt->second->m_done) {
      // assume all exist in the chunk stack
      Chunk *chunksInStack[CHUNKS_PER_STACK]{};
      for (int z = 0; z < CHUNKS_PER_STACK; z++) {
        chunksInStack[z] = getChunkRawPtr({posIt->first.x, posIt->first.y, z});
      }
      posIt->second->applyTerrainDataToChunks(chunksInStack);
      m_chunkHeightMapMap.emplace(posIt->first, posIt->second->m_heightMap);
      m_chunkTreeMapMap.emplace(posIt->first, posIt->second->m_treeMap);

      // delete from map regardless of whether chunk exists
      posIt = m_chunkTerrainLoadInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }

  for (auto posIt = m_chunkStructuresInfoMap.begin(); posIt != m_chunkStructuresInfoMap.end();) {
    if (posIt->second->chunkState == ChunkState::STRUCTURES_GENERATED) {
      posIt = m_chunkStructuresInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }
}

void World::combinedUpdateListsTEST() {
  if (m_centerChangedXY || m_opaqueRenderSet.empty()) {
    int loadBoundMinX = m_center.x - m_loadDistance;
    int loadBoundMaxX = m_center.x + m_loadDistance;
    int loadBoundMinY = m_center.y - m_loadDistance;
    int loadBoundMaxY = m_center.y + m_loadDistance;
    int structureBoundMinX = loadBoundMinX + 1;
    int structureBoundMinY = loadBoundMinY + 1;
    int structureBoundMaxX = loadBoundMaxX - 1;
    int structureBoundMaxY = loadBoundMaxY - 1;
    m_chunksInStructureGenRangeVectorXY.clear();
    glm::ivec2 pos;
    for (pos.x = loadBoundMinX; pos.x <= loadBoundMaxX; pos.x++) {
      for (pos.y = loadBoundMinY; pos.y <= loadBoundMaxY; pos.y++) {
        // Terrain generation check
        Chunk *chunkBottom = getChunkRawPtr({pos.x, pos.y, 0});
        if (chunkBottom->chunkState == ChunkState::UNGENERATED) {
          if (!m_chunkTerrainLoadInfoMap.count(pos)) {
            m_chunksToLoadVector.emplace_back(pos);
            m_chunkTerrainLoadInfoMap.emplace(pos, std::make_unique<ChunkTerrainInfo>(pos, m_terrainGenerator));
            continue;
          }
        } else if (chunkBottom->chunkState == ChunkState::TERRAIN_GENERATED) {
          if (pos.x >= structureBoundMinX && pos.x <= structureBoundMaxX
              && pos.y >= structureBoundMinY && pos.y <= structureBoundMaxY &&
              !m_chunkStructuresInfoMap.count({pos.x, pos.y, 0})) {
            m_chunksInStructureGenRangeVectorXY.emplace_back(pos);
          }
        }

        // Structure generation check

        // Lighting check

        // Meshing check

      }
      std::sort(m_chunksToLoadVector.begin(), m_chunksToLoadVector.end(), rcmpVec2);
      std::sort(m_chunksInStructureGenRangeVectorXY.begin(), m_chunksInStructureGenRangeVectorXY.end(), rcmpVec2);
    }
  }
}

void World::combinedUpdateSortedTEST() {
  for (auto posIt = m_chunksInStructureGenRangeVectorXY.begin(); posIt != m_chunksInStructureGenRangeVectorXY.end();) {
    for (int z = 0; z < CHUNKS_PER_STACK; z++) {
      auto pos = glm::ivec3(posIt->x, posIt->y, z);
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
      Chunk *chunks[27] = {nullptr};
      getNeighborChunks(chunks, pos);

      bool canGenStructures = true;
      for (auto &chunk : chunks) {
        if (chunk && chunk->chunkState != ChunkState::TERRAIN_GENERATED
            && chunk->chunkState != ChunkState::STRUCTURES_GENERATED &&
            chunk->chunkState != ChunkState::FULLY_GENERATED) {
          canGenStructures = false;
          break;
        }
      }

      if (canGenStructures) {
        // TODO move this logic of adding neighbor chunks to the chunk somewhere else? or at least a new function
        Chunk *chunk = chunks[13];
        for (int i = 0; i < 27; i++) {
          chunk->m_neighborChunks[i] = chunks[i];
        }
        m_chunkStructuresInfoMap.emplace(pos, chunk);
        auto insertPos = std::lower_bound(
            m_chunksReadyToGenStructuresList.begin(), m_chunksReadyToGenStructuresList.end(), *posIt, rcmpVec2);
        m_chunksReadyToGenStructuresList.insert(insertPos, pos);
      }
    }
    posIt = m_chunksInStructureGenRangeVectorXY.erase(posIt);
  }
}

void World::updateChunkLoadList() {
  // apply terrain and update map regardless of whether pos changed
  for (auto posIt = m_chunkTerrainLoadInfoMap.begin(); posIt != m_chunkTerrainLoadInfoMap.end();) {
    if (posIt->second->m_done) {
      // assume all exist in the chunk stack
      Chunk *chunksInStack[CHUNKS_PER_STACK]{};
      for (int z = 0; z < CHUNKS_PER_STACK; z++) {
        chunksInStack[z] = getChunkRawPtr({posIt->first.x, posIt->first.y, z});
      }
      posIt->second->applyTerrainDataToChunks(chunksInStack);
      m_chunkHeightMapMap.emplace(posIt->first, posIt->second->m_heightMap);
      m_chunkTreeMapMap.emplace(posIt->first, posIt->second->m_treeMap);

      // delete from map regardless of whether chunk exists
      posIt = m_chunkTerrainLoadInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }

  glm::ivec2 pos;
  for (pos.x = m_center.x - m_loadDistance; pos.x <= m_center.x + m_loadDistance; pos.x++) {
    for (pos.y = m_center.y - m_loadDistance; pos.y <= m_center.y + m_loadDistance; pos.y++) {
      if (m_chunkMap.at({pos.x, pos.y, 0})->chunkState == ChunkState::UNGENERATED
          && !m_chunkTerrainLoadInfoMap.count(pos)) {
        m_chunksToLoadVector.emplace_back(pos);
        m_chunkTerrainLoadInfoMap.emplace(pos, std::make_unique<ChunkTerrainInfo>(pos, m_terrainGenerator));
      }
    }
    std::sort(m_chunksToLoadVector.begin(), m_chunksToLoadVector.end(), rcmpVec2);
  }
}

void World::updateChunkStructureGenList() {
  for (auto posIt = m_chunkStructuresInfoMap.begin(); posIt != m_chunkStructuresInfoMap.end();) {
    if (posIt->second->chunkState == ChunkState::STRUCTURES_GENERATED) {
      posIt = m_chunkStructuresInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }

  if (m_centerChangedXY || m_opaqueRenderSet.empty()) {
    m_chunksInStructureGenRangeVectorXY.clear();
    glm::ivec2 pos;
    for (pos.x = m_center.x - m_structureLoadDistance; pos.x <= m_center.x + m_structureLoadDistance; pos.x++) {
      for (pos.y = m_center.y - m_structureLoadDistance; pos.y <= m_center.y + m_structureLoadDistance; pos.y++) {
        if (m_chunkMap.at({pos.x, pos.y, 0})->chunkState == ChunkState::TERRAIN_GENERATED
            && !m_chunkStructuresInfoMap.count({pos.x, pos.y, 0})) {
          m_chunksInStructureGenRangeVectorXY.emplace_back(pos);
        }
      }
    }
    std::sort(m_chunksInStructureGenRangeVectorXY.begin(), m_chunksInStructureGenRangeVectorXY.end(), rcmpVec2);
  }

  for (auto posIt = m_chunksInStructureGenRangeVectorXY.begin(); posIt != m_chunksInStructureGenRangeVectorXY.end();) {
    for (int z = 0; z < CHUNKS_PER_STACK; z++) {
      auto pos = glm::ivec3(posIt->x, posIt->y, z);

      Chunk *chunks[27] = {nullptr};
      getNeighborChunks(chunks, pos);

      bool canGenStructures = true;
      for (auto &chunk : chunks) {
        if (chunk && chunk->chunkState != ChunkState::TERRAIN_GENERATED
            && chunk->chunkState != ChunkState::STRUCTURES_GENERATED &&
            chunk->chunkState != ChunkState::FULLY_GENERATED) {
          canGenStructures = false;
          break;
        }
      }

      if (canGenStructures) {
        // TODO move this logic of adding neighbor chunks to the chunk somewhere else? or at least a new function
        Chunk *chunk = chunks[13];
        for (int i = 0; i < 27; i++) {
          chunk->m_neighborChunks[i] = chunks[i];
        }
        m_chunkStructuresInfoMap.emplace(pos, chunk);
        auto insertPos = std::lower_bound(
            m_chunksReadyToGenStructuresList.begin(), m_chunksReadyToGenStructuresList.end(), *posIt, rcmpVec2);
        m_chunksReadyToGenStructuresList.insert(insertPos, pos);
      }
    }
    posIt = m_chunksInStructureGenRangeVectorXY.erase(posIt);
  }
}

void World::updateChunkLightingList() {
  for (auto iter = m_chunkStacksToLightMap.begin(); iter != m_chunkStacksToLightMap.end();) {
    bool shouldErase = true;
    for (int chunkIndex = 0; chunkIndex < CHUNKS_PER_STACK; chunkIndex++) {
      if (iter->second[chunkIndex]->chunkState != ChunkState::FULLY_GENERATED) {
        shouldErase = false;
        break;
      }
    }
    if (shouldErase) {
      iter = m_chunkStacksToLightMap.erase(iter);
    } else {
      iter++;
    }
  }

  if (m_centerChangedXY || m_opaqueRenderSet.empty()) {
    m_chunkStackPositionsEligibleForLighting.clear();
    glm::ivec3 pos;
    for (pos.x = m_center.x - m_lightingLoadDistance; pos.x <= m_center.x + m_lightingLoadDistance; pos.x++) {
      for (pos.y = m_center.y - m_lightingLoadDistance; pos.y <= m_center.y + m_lightingLoadDistance; pos.y++) {
        if (m_chunkStacksToLightMap.count({pos.x, pos.y})) continue;
        bool canLight = true;
        for (pos.z = 0; pos.z < CHUNKS_PER_STACK; pos.z++) {
          if (m_chunkMap.at(pos)->chunkState != ChunkState::STRUCTURES_GENERATED) {
            canLight = false;
            break;
          }
        }
        if (canLight) m_chunkStackPositionsEligibleForLighting.emplace_back(pos);
      }
    }
    std::sort(m_chunkStackPositionsEligibleForLighting.begin(),
              m_chunkStackPositionsEligibleForLighting.end(),
              rcmpVec2);
  }

  for (auto posIt = m_chunkStackPositionsEligibleForLighting.begin();
       posIt != m_chunkStackPositionsEligibleForLighting.end();) {
    glm::ivec2 pos2d = *posIt;
    ChunkStackArray chunks = {nullptr};
//    Chunk *chunks[CHUNKS_PER_STACK] = {nullptr};
    bool canLight = true;
    for (glm::ivec3 pos = {pos2d.x, pos2d.y, 0}; pos.z < CHUNKS_PER_STACK; pos.z++) {
      Chunk *chunkToLight = getChunkRawPtr(pos);
      // check horizontal neighbors

      // should only calculate light when block placement is done for all neighbors, which includes
      // neighbors that already have lighting completed
      for (auto neighborIndex : Chunk::HORIZONTAL_NEIGHBOR_INDICES) {
        ChunkState horizontalNeighborChunkState = chunkToLight->m_neighborChunks[neighborIndex]->chunkState;
        if (horizontalNeighborChunkState != ChunkState::STRUCTURES_GENERATED
            && horizontalNeighborChunkState != ChunkState::FULLY_GENERATED) {
          canLight = false;
          break;
        }
      }
      if (!canLight) {
        break;
      }
      chunks[pos.z] = chunkToLight;
    }
    if (canLight) {
      m_chunkStacksToLightMap.emplace(pos2d, chunks);
      auto insertPos = std::lower_bound(m_chunkStackPositionsToLightList.begin(),
                                        m_chunkStackPositionsToLightList.end(),
                                        pos2d,
                                        rcmpVec2);
      m_chunkStackPositionsToLightList.insert(insertPos, pos2d);
//      std::cout << pos2d.x << " " << pos2d.y << std::endl;
      posIt = m_chunkStackPositionsEligibleForLighting.erase(posIt);
    } else {
      posIt++;
    }
  }
}

void World::updateChunkMeshList() {
  // apply meshes and update map regardless of whether pos changed
  for (auto posIt = m_chunkMeshInfoMap.begin(); posIt != m_chunkMeshInfoMap.end();) {
    if (posIt->second->m_done) {
      Chunk *chunk = getChunkRawPtr(posIt->first);
      posIt->second->applyMeshDataToMesh();
      if (!chunk->m_opaqueMesh.vertices.empty()) m_opaqueRenderSet.insert(posIt->first);
      if (!chunk->m_transparentMesh.vertices.empty()) m_transparentRenderSet.insert(posIt->first);
      // delete from map regardless of whether chunk exists
      posIt = m_chunkMeshInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }

  m_chunkPositionsEligibleForMeshing.clear();
  // add chunks that can be meshed into vector
  glm::ivec3 pos;
  for (pos.x = m_center.x - m_renderDistance; pos.x <= m_center.x + m_renderDistance; pos.x++) {
    for (pos.y = m_center.y - m_renderDistance; pos.y <= m_center.y + m_renderDistance; pos.y++) {
      for (pos.z = 0; pos.z < CHUNKS_PER_STACK; pos.z++) {
        Chunk *chunk = getChunkRawPtr(pos);
        if (chunk->m_numNonAirBlocks != 0 && chunk->chunkMeshState != ChunkMeshState::BUILT
            && chunk->chunkState == ChunkState::FULLY_GENERATED && !m_chunkMeshInfoMap.count(pos)) {
          m_chunkPositionsEligibleForMeshing.emplace_back(pos);
        }
      }
    }
  }
  m_chunksReadyToMeshList.sort(rcmpVec3);

  for (auto posIt = m_chunkPositionsEligibleForMeshing.begin(); posIt != m_chunkPositionsEligibleForMeshing.end();) {
    Chunk *chunkToMesh = getChunkRawPtr(*posIt);

    bool canMesh = true;
    for (auto &chunk : chunkToMesh->m_neighborChunks) {
      if (chunk && chunk->chunkState != ChunkState::FULLY_GENERATED) {
        canMesh = false;
//        if (chunk->m_pos.x < m_center.x + m_renderDistance && chunk->m_pos.x > m_center.x - m_renderDistance
//            && chunk->m_pos.y < m_center.y + m_renderDistance && chunk->m_pos.y > m_center.y - m_renderDistance &&  m_chunksReadyToGenStructuresList.empty()) {
//          std::cout << "cant mesh because: " << chunk->m_pos.x << " " << chunk->m_pos.y << " " << chunk->m_pos.z;
//          if (chunk->chunkState == ChunkState::STRUCTURES_GENERATED) {
//            std::cout << "  structs generated but not light\n\n";
//          }
//        }
        break;
      }
    }

    if (canMesh) {
      m_chunkMeshInfoMap.emplace(*posIt, std::make_unique<ChunkMeshInfo>(chunkToMesh));
      auto insertPos = std::lower_bound(m_chunksReadyToMeshList.begin(), m_chunksReadyToMeshList.end(),
                                        *posIt, rcmpVec3);
      m_chunksReadyToMeshList.insert(insertPos, *posIt);
      posIt = m_chunkPositionsEligibleForMeshing.erase(posIt);
    } else {
      posIt++;
    }
  }
}

void World::unloadChunks() {
  for (auto it = m_chunkMap.begin(); it != m_chunkMap.end();) {
    auto pos = it->first;
    if (pos.x < m_center.x - m_unloadDistance ||
        pos.x > m_center.x + m_unloadDistance ||
        pos.y < m_center.y - m_unloadDistance ||
        pos.y > m_center.y + m_unloadDistance) {
      m_opaqueRenderSet.erase(pos);
      m_transparentRenderSet.erase(pos);
      m_chunkHeightMapMap.erase(pos);
      m_chunkTreeMapMap.erase(pos);
      it = m_chunkMap.erase(it);
    } else {
      ++it;
    }
  }
}

void World::generateChunksWorker4() {
  while (true) {
    std::queue<glm::ivec2> batchToLoad;
    std::queue<glm::ivec3> batchToGenStructures;
    std::queue<glm::ivec2> batchToLight;
    std::queue<glm::ivec3> batchToMesh;

    std::unique_lock<std::mutex> lock(m_mainMutex);
    m_conditionVariable.wait(lock, [this]() {
      return !m_isRunning || (m_numRunningThreads < m_numLoadingThreads
          && (!m_chunksToLoadVector.empty() || !m_chunksReadyToGenStructuresList.empty() ||
              !m_chunkStackPositionsToLightList.empty() ||
              !m_chunksReadyToMeshList.empty()));
    });
    if (!m_isRunning) return;
    if (!m_chunksToLoadVector.empty()) {
      while (!m_chunksToLoadVector.empty() && batchToLoad.size() < 10) {
        batchToLoad.push(m_chunksToLoadVector.back());
        m_chunksToLoadVector.pop_back();
      }
      lock.unlock();
      processBatchToLoad(batchToLoad);
    } else if (!m_chunksReadyToGenStructuresList.empty() && m_chunksToLoadVector.empty()) {
      while (!m_chunksReadyToGenStructuresList.empty() && batchToGenStructures.size() < 10) {
        batchToGenStructures.push(m_chunksReadyToGenStructuresList.back());
        m_chunksReadyToGenStructuresList.pop_back();
      }
      lock.unlock();
      processBatchToGenStructures(batchToGenStructures);
    } else if (!m_chunkStackPositionsToLightList.empty() && m_chunksReadyToGenStructuresList.empty() &&
        m_chunksToLoadVector.empty()) {
      while (!m_chunkStackPositionsToLightList.empty() && batchToLight.size() < 10) {
        batchToLight.push(m_chunkStackPositionsToLightList.back());
        m_chunkStackPositionsToLightList.pop_back();
      }
      lock.unlock();
      processBatchToLight(batchToLight);
    } else if (!m_chunksReadyToMeshList.empty() && m_chunksReadyToGenStructuresList.empty()
        && m_chunksToLoadVector.empty()) {
      while (!m_chunksReadyToMeshList.empty() && batchToMesh.size() < MAX_BATCH_SIZE) {
        auto pos = m_chunksReadyToMeshList.begin();
        batchToMesh.push(m_chunksReadyToMeshList.back());
        m_chunksReadyToMeshList.pop_back();
      }
      lock.unlock();
      processBatchToMesh(batchToMesh);
    }
  }
}

void World::processBatchToLoad(std::queue<glm::ivec2> &batchToLoad) {
  while (!batchToLoad.empty()) {
    const auto &pos = batchToLoad.front();
    auto it = m_chunkTerrainLoadInfoMap.find(pos);
    if (it != m_chunkTerrainLoadInfoMap.end()) it->second->generateTerrainData();
    batchToLoad.pop();
  }
}

void World::processBatchToGenStructures(std::queue<glm::ivec3> &batchToGenStructures) {
  while (!batchToGenStructures.empty()) {
    const auto &pos = batchToGenStructures.front();

    // find instead of at
    auto heightMapIt = m_chunkHeightMapMap.find(pos);
    auto treeMapIt = m_chunkTreeMapMap.find(pos);
    if (heightMapIt == m_chunkHeightMapMap.end() || treeMapIt == m_chunkTreeMapMap.end()) {
      std::cout << "BROKEN HEIGHTMAP OR TREEMAP" << std::endl;
    } else {
      auto it = m_chunkStructuresInfoMap.find(pos);
      if (it != m_chunkStructuresInfoMap.end()) {
        TerrainGenerator::generateStructures(it->second, heightMapIt->second, treeMapIt->second);
      }
    }
//    m_chunkStructuresInfoMap.at(pos)->generateStructureData(heightMapIt->second, treeMapIt->second);
    batchToGenStructures.pop();
  }
}

void World::processBatchToLight(std::queue<glm::ivec2> &batchToLight) {
  while (!batchToLight.empty()) {
    const auto pos = batchToLight.front();
    auto it = m_chunkStacksToLightMap.find(pos);
    if (it != m_chunkStacksToLightMap.end()) {
//      bool fl = true;
//      for (int i = 0; i < 8; i++) {
//        if (it->second[i]->chunkState != ChunkState::STRUCTURES_GENERATED) {
//          fl = false;
//          break;
//        }
//      }
//      if (fl) {
//        ChunkAlg::generateLightData(it->second);
//      }
        ChunkAlg::generateLightData(it->second);

    }
    batchToLight.pop();
  }
}

void World::processBatchToMesh(std::queue<glm::ivec3> &batchToMesh) {
  while (!batchToMesh.empty()) {
    const auto &pos = batchToMesh.front();
    auto it = m_chunkMeshInfoMap.find(pos);
    if (it != m_chunkMeshInfoMap.end()) it->second->generateMeshData();
//    m_chunkMeshInfoMap.at(pos)->generateMeshData();
    batchToMesh.pop();
  }
}

bool compareVec3(glm::vec3 a, glm::vec3 b) {
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

void World::castPlayerAimRay(Ray ray) {
  glm::ivec3 lastAirBlockPos = NULL_VECTOR;
  glm::vec3 rayStart = ray.origin;
  glm::vec3 rayEnd = ray.origin;
  glm::vec3 directionIncrement = glm::normalize(ray.direction) * 0.05f;
  static std::chrono::steady_clock::time_point
      lastTime = std::chrono::steady_clock::now();
  static bool isFirstAction = true;

  while (glm::distance(rayStart, rayEnd) < 10.0f) {
    glm::ivec3 blockPos = {floor(rayEnd.x), floor(rayEnd.y), floor(rayEnd.z)};
    try {
      Block block = getBlockFromWorldPosition(blockPos);
      if (block == Block::AIR) {
        lastAirBlockPos = blockPos;
        rayEnd += directionIncrement;
        continue;
      }
    } catch (std::exception &e) {
      return;
    }

    m_prevLastRayCastBlockPos = m_lastRayCastBlockPos;
    m_lastRayCastBlockPos = blockPos;

    // calculate block break stage. 10 stages. 0 is no break, 10 is fully broken
    auto duration = std::chrono::steady_clock::now() - lastTime;
    auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(
        duration).count();
    int breakStage = static_cast<int>(durationMS / (MINING_DELAY_MS / 10));
    if (breakStage > 10) breakStage = 10;

    // breaking block
    if (Mouse::isPressed(GLFW_MOUSE_BUTTON_LEFT)) {
      // check if player switched block aimed at. if so, reset time and break stage and return
      if (!compareVec3(m_lastRayCastBlockPos, m_prevLastRayCastBlockPos)) {
        player.blockBreakStage = 0;
        lastTime = std::chrono::steady_clock::now();
        return;
      }
      if (std::chrono::steady_clock::now() - lastTime <
          std::chrono::milliseconds(MINING_DELAY_MS)) {
        player.blockBreakStage = breakStage;
        return;
      }

      setBlockWithUpdate({blockPos.x, blockPos.y, blockPos.z},
                         Block::AIR);
      player.blockBreakStage = 0;
      m_lastRayCastBlockPos = NULL_VECTOR;
      isFirstAction = false;
    }
      // placing block
    else if (Mouse::isPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
      if (!isFirstAction && std::chrono::steady_clock::now() - lastTime <
          std::chrono::milliseconds(PLACING_DELAY_MS)) {
        return;
      }
      setBlockWithUpdate(lastAirBlockPos,
                         Block(player.inventory.getHeldItem()));
      isFirstAction = false;

      // not placing or breaking, reset
    } else {
      player.blockBreakStage = 0;
      isFirstAction = true;
    }

    lastTime = std::chrono::steady_clock::now();
    return;
  }
  lastTime = std::chrono::steady_clock::now();
  player.blockBreakStage = 0;
  m_lastRayCastBlockPos = NULL_VECTOR;
}

void World::renderDebugGui() {
  ImGuiIO &io = ImGui::GetIO();

  ImGui::Begin("Debug");

  ImGui::Text("Framerate: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
              io.Framerate);

  int renderDistance = m_renderDistance;
  if (ImGui::SliderInt("Render Distance", &renderDistance, 1, 16)) {
    setRenderDistance(renderDistance);
  }

  bool useAmbientOcclusion = Config::getUseAmbientOcclusion();
  if (ImGui::Checkbox("Ambient Occlusion", &useAmbientOcclusion)) {
    Config::setUseAmbientOcclusion(useAmbientOcclusion);
//    m_renderer.updateShaderUniforms();
  }

  bool useWireFrame = Config::useWireFrame;
  if (ImGui::Checkbox("WireFrame", &useWireFrame)) {
    Config::useWireFrame = useWireFrame;
  }

  ImGui::Text("lastRayCastBlockPos: %d, %d, %d", m_lastRayCastBlockPos.x,
              m_lastRayCastBlockPos.y, m_lastRayCastBlockPos.z);

  ImGui::Text("Running Threads: %d", static_cast<int>(m_numRunningThreads));

  ImGui::Text("ChunksToLoadVector: %d", static_cast<int>(m_chunksToLoadVector.size()));
  ImGui::Text("ChunkTerrainLoadInfoMap: %d", static_cast<int>(m_chunkTerrainLoadInfoMap.size()));
  ImGui::Text("ChunksInStructureGenRangeVectorXY: %d", static_cast<int>(m_chunksInStructureGenRangeVectorXY.size()));
  ImGui::Text("ChunkStructuresInfoMap: %d", static_cast<int>(m_chunkStructuresInfoMap.size()));
  ImGui::Text("ChunksReadyToGenStructuresList: %d", static_cast<int>(m_chunksReadyToGenStructuresList.size()));
  ImGui::Text("ChunksInLightingRangeVectorXY: %d", static_cast<int>(m_chunkStackPositionsEligibleForLighting.size()));
  ImGui::Text("ChunkLightInfoMap: %d", static_cast<int>(m_chunkStacksToLightMap.size()));
  ImGui::Text("ChunksReadyForLightingList: %d", static_cast<int>(m_chunkStackPositionsToLightList.size()));
  ImGui::Text("ChunksInMeshRangeVector: %d", static_cast<int>(m_chunkPositionsEligibleForMeshing.size()));
  ImGui::Text("ChunkMeshInfoMap: %d", static_cast<int>(m_chunkMeshInfoMap.size()));
  ImGui::Text("ChunksReadyToMeshList: %d", static_cast<int>(m_chunksReadyToMeshList.size()));
  ImGui::Text("ChunkUpdateVector: %d", static_cast<int>(m_chunkUpdateVector.size()));
  ImGui::Text("ChunkUpdateInfoMap: %d", static_cast<int>(m_chunkUpdateInfoMap.size()));
  ImGui::Text("ChunkDirectlyUpdateSet: %d", static_cast<int>(m_chunkDirectlyUpdateSet.size()));
  ImGui::Text("OpaqueRenderSet: %d", static_cast<int>(m_opaqueRenderSet.size()));
  ImGui::Text("TransparentRenderSet: %d", static_cast<int>(m_transparentRenderSet.size()));
  ImGui::Text("TransparentRenderVector: %d", static_cast<int>(m_transparentRenderVector.size()));
  ImGui::Text("HeightMapMap: %d", static_cast<int>(m_chunkHeightMapMap.size()));

  player.renderDebugGui();

  ImGui::End();
}

void World::saveData() {
  m_worldSave.saveData();
}

Block World::getBlockFromWorldPosition(const glm::ivec3 &position) const {
  auto chunkPos = chunkPosFromWorldPos(position.x, position.y, position.z);
  Chunk *chunk = getChunkRawPtr(chunkPos);
  int chunkX = Utils::positiveModulo(position.x, CHUNK_SIZE);
  int chunkY = Utils::positiveModulo(position.y, CHUNK_SIZE);
  int chunkZ = Utils::positiveModulo(position.z, CHUNK_SIZE);
  return chunk->getBlock(chunkX, chunkY, chunkZ);
}

void World::setRenderDistance(int renderDistance) {
  m_renderDistance = renderDistance;
  m_structureLoadDistance = m_renderDistance + 1;
  m_lightingLoadDistance = m_renderDistance + 2;
  m_loadDistance = m_renderDistance + 3;

  m_unloadDistance = m_renderDistance + 4;
  m_renderDistanceChanged = true;
}

void World::setBlockWithUpdate(const glm::ivec3 &worldPos, Block block) {
  bool lightChanged = false;

  auto chunkPos = chunkPosFromWorldPos(worldPos);
  auto blockPosInChunk = worldPos - chunkPos * CHUNK_SIZE;
//  Chunk &chunk = *m_chunkMap.at(chunkPos).get();
  Chunk *chunk = getChunkRawPtr(chunkPos);
  Block oldBlock = chunk->getBlock(blockPosInChunk);
  uint16_t oldTorchLightPacked = BlockDB::getpackedLightLevel(oldBlock);
  uint16_t newTorchLightPacked = BlockDB::getpackedLightLevel(block);
  glm::ivec3 oldTorchLight = Utils::unpackLightLevel(oldTorchLightPacked);
  // cases
  // 1) torch of any color to air.
  if (block == Block::AIR && BlockDB::isLightSource(oldBlock)) {
    m_torchlightRemovalQueue.emplace(blockPosInChunk, oldTorchLightPacked);
    chunk->setTorchLevel(blockPosInChunk, 0);
    lightChanged = true;
  }
    // 2) air to any color
  else if (oldBlock == Block::AIR && newTorchLightPacked > 0) {
    m_torchLightPlacementQueue.emplace(blockPosInChunk, newTorchLightPacked);
    chunk->setTorchLevel(blockPosInChunk, newTorchLightPacked);
    lightChanged = true;
  } else {
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(blockPosInChunk, faceNum);
      uint16_t neighborLightLevel = chunk->getTorchLevelPackedIncludingNeighborsOptimized(neighborPos);
      if (neighborLightLevel) {
        m_torchLightPlacementQueue.emplace(neighborPos, neighborLightLevel);
//      lightChanged = true;
      }
    }
  }
  // 3) air to not torch
  // 4) not torch to air


  ChunkAlg::unpropagateTorchLight(m_torchLightPlacementQueue, m_torchlightRemovalQueue, chunk);
  chunk->setBlock(blockPosInChunk, block);
  ChunkAlg::propagateTorchLight(m_torchLightPlacementQueue, chunk);

  if (lightChanged) {
    // if light changed add all the neighbor chunks.
    for (auto &c : chunk->m_neighborChunks) {
      if (c) m_chunkDirectlyUpdateSet.insert(c->m_pos);
    }
  } else {
    // if light didnt change, calculate which chunks to add since high chance a block was changed not on the border
    addRelatedChunks(blockPosInChunk, chunkPos, m_chunkDirectlyUpdateSet);
  }
}

void World::processDirectChunkUpdates() {
  if (m_chunkDirectlyUpdateSet.empty()) return;

  for (const glm::ivec3 &pos : m_chunkDirectlyUpdateSet) {
    Chunk *chunk = getChunkRawPtrOrNull(pos);
    if (!chunk) continue;
    if (chunk->chunkMeshState != ChunkMeshState::BUILT) continue; // When will this happen realistically?

    std::vector<ChunkVertex> opaqueVertices, transparentVertices;
    std::vector<unsigned int> opaqueIndices, transparentIndices;

    Block blocks[CHUNK_MESH_INFO_SIZE]{}; // NEED TO INITIALIZE
    uint16_t torchLightLevels[CHUNK_MESH_INFO_SIZE]{}; // NEED TO INITIALIZE
    uint8_t sunlightLevels[CHUNK_MESH_INFO_SIZE]{}; // NEED TO INITIALIZE
    ChunkMeshInfo::populateMeshInfoForMeshing(blocks, torchLightLevels, sunlightLevels, chunk->m_neighborChunks);
    ChunkMeshBuilder builder(blocks, torchLightLevels, sunlightLevels, chunk->m_worldPos);
    builder.constructMesh(opaqueVertices, opaqueIndices, transparentVertices, transparentIndices);

    chunk->m_opaqueMesh.vertices = std::move(opaqueVertices);
    chunk->m_opaqueMesh.indices = std::move(opaqueIndices);
    chunk->m_transparentMesh.vertices = std::move(transparentVertices);
    chunk->m_transparentMesh.indices = std::move(transparentIndices);
    chunk->m_opaqueMesh.needsUpdate = true;
    chunk->m_transparentMesh.needsUpdate = true;
  }
  m_chunkDirectlyUpdateSet.clear();
}

void World::sortTransparentRenderVector() {
  m_transparentRenderVector = std::vector<glm::ivec3>(m_transparentRenderSet.begin(), m_transparentRenderSet.end());
  std::sort(m_transparentRenderVector.begin(), m_transparentRenderVector.end(), rcmpVec3);
}

void World::getNeighborChunks(Chunk *(&chunks)[27], const glm::ivec3 &pos) const {
  int index = 0;
  for (auto &neighborOffset : NEIGHBOR_ARRAY_OFFSETS) {
    auto neighborPos = pos + neighborOffset;
    if (neighborPos.z < 0 || neighborPos.z >= CHUNKS_PER_STACK) {
      chunks[index++] = nullptr;
    } else {
      chunks[index++] = getChunkRawPtr(pos + neighborOffset);
    }
  }
}

void World::addRelatedChunks(const glm::ivec3 &blockPosInChunk,
                             const glm::ivec3 &chunkPos,
                             std::unordered_set<glm::ivec3> &chunkSet) {
  glm::ivec3 chunksToAdd[8]; // at most 8 chunks are related to a block
  glm::ivec3 temp; // temp variable to store the chunk to add (calculate offset from chunk pos)
  int numChunksToAdd = 1; // always add the chunk the block is in
  int _size;
  chunksToAdd[0] = chunkPos; // always add the chunk the block is in

  // iterate over each axis
  for (int axis = 0; axis < 3; axis++) {
    // if block is on edge of the axis, add the chunko on the other side of the edge
    if (blockPosInChunk[axis] == 0) {
      _size = numChunksToAdd;
      for (int i = 0; i < _size; i++) {
        temp = chunksToAdd[i]; // works since only doing one axis at a time
        temp[axis]--; // decrement chunk pos on the axis
        chunksToAdd[numChunksToAdd++] = temp;
      }
    }

    if (blockPosInChunk[axis] == CHUNK_SIZE - 1) {
      _size = numChunksToAdd;
      for (int i = 0; i < _size; i++) {
        temp = chunksToAdd[i];
        temp[axis]++;
        chunksToAdd[numChunksToAdd++] = temp;
      }
    }
  }

  // add the chunks to the set
  for (int i = 0; i < numChunksToAdd; i++) {
    chunkSet.insert(chunksToAdd[i]);
  }
}



