//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.hpp"
#include "../Config.hpp"
#include "block/BlockDB.hpp"
#include "../input/Mouse.hpp"
#include "../utils/Utils.hpp"
#include "../utils/Timer.hpp"
#include <algorithm>

World::World(Renderer &renderer, int seed, const std::string &savePath)
    : m_worldSave(savePath), m_renderer(renderer),
      chunkRenderer(player.camera),
      m_center(INT_MAX), m_numRunningThreads(0),
      m_numLoadingThreads(std::thread::hardware_concurrency()),
      m_seed(seed) {

  const size_t loadVectorSize =
      ((size_t) (m_renderDistance + 2) * 2 + 1)
          * ((size_t) (m_renderDistance + 2) * 2 + 1);
  m_chunksToLoadVector.reserve(loadVectorSize);

  BlockDB::loadData("../resources/blocks/");

  for (int i = 0; i < m_numLoadingThreads; i++) {
    m_chunkLoadThreads.emplace_back(&World::generateTerrainWorker, this);
    m_chunkLoadThreads.emplace_back(&World::generateStructuresWorker, this);
    m_chunkLoadThreads.emplace_back(&World::generateChunkMeshWorker, this);
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

void World::render() {
  chunkRenderer.start();
  for (auto &chunkKey : m_renderSet) {
    if (chunkExists(chunkKey)) {
      chunkRenderer.render(*m_chunkMap.at(chunkKey));
    }
  }

  // render block break and outline if a block is being aimed at
  if (static_cast<const glm::vec3>(m_lastRayCastBlockPos) != NULL_VECTOR) {
    m_renderer.renderBlockOutline(player.camera, m_lastRayCastBlockPos);
    m_renderer.renderBlockBreak(player.camera,
                                m_lastRayCastBlockPos,
                                player.blockBreakStage);
  }
  m_renderer.renderCrossHair();
  renderDebugGui();
}

void World::update() {
//    std::cout << "chunksToLoadVector size: " << m_chunksToLoadVector.size()
//            << std::endl;
//  std::cout << "chunksReadyToMeshList size: "
//            << m_chunksReadyToMeshList.size() << std::endl;
//  std::cout << "chunksReadyToGenStructuresList size: "
//            << m_chunksReadyToGenStructuresList.size() << std::endl;
//  std::cout << "chunksInStructureGenRangeVector size: "
//            << m_chunksInStructureGenRangeVector.size() << std::endl;
//  std::cout << "chunksInMeshRangeVector size: "
//            << m_chunksInMeshRangeVector.size() << std::endl;
//  std::cout << "chunkTerrainLoadInfoMap size: "
//            << m_chunkTerrainLoadInfoMap.size() << std::endl;
//  std::cout << "chunkStructureGenInfoMap size: "
//            << m_chunkStructureGenInfoMap.size() << std::endl;
//  std::cout << "chunkMeshInfoMap size: " << m_chunkMeshInfoMap.size()
//            << std::endl;

  m_xyChanged = false;
  // update center and whether position changed
  auto playerChunkPos = player.getChunkPosition();
  auto newCenter = glm::ivec2(playerChunkPos.x, playerChunkPos.y);
  if (newCenter != m_center) {
    m_xyChanged = true;
    m_center = newCenter;
  }

  if (m_xyChanged) {
    // create chunks in range (not terrain generated at this point)
    glm::ivec2 pos;
    for (pos.x = m_center.x - m_loadDistance;
         pos.x <= m_center.x + m_loadDistance;
         pos.x++) {
      for (pos.y = m_center.y - m_loadDistance;
           pos.y <= m_center.y + m_loadDistance; pos.y++) {
        if (chunkExists(pos)) continue; // skip non existent chunks
        m_chunkMap.emplace(pos, std::make_unique<Chunk>(pos));
      }
    }

    unloadChunks();
  }

  castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});

  // chunk updates

  // if have chunks to remesh, lock
//    if (m_chunkManager.hasChunksToRemesh()) {
//        std::lock_guard<std::mutex> lock(m_mainMutex);
//        m_chunkManager.remeshChunksToRemesh();
//    }
  static int i = 0;

  std::lock_guard<std::mutex> lock(m_mainMutex);
  updateChunkLoadList();
  updateChunkStructureGenList();
  updateChunkMeshList();

  m_conditionVariable.notify_all();
}

void World::updateChunkLoadList() {
  // apply terrain and update map regardless of whether pos changed
  for (auto chunkKeyIter = m_chunkTerrainLoadInfoMap.begin();
       chunkKeyIter != m_chunkTerrainLoadInfoMap.end();) {
    if (chunkKeyIter->second->m_done) {
      if (chunkExists(chunkKeyIter->first)) {
        chunkKeyIter->second->applyTerrain(getChunkRawPtr(chunkKeyIter->first));
      }
      // delete from map regardless of whether chunk exists
      chunkKeyIter = m_chunkTerrainLoadInfoMap.erase(chunkKeyIter);
    } else {
      chunkKeyIter++;
    }
  }

  if (m_xyChanged || m_renderSet.empty()) {
    glm::ivec2 pos;
    for (pos.x = m_center.x - m_loadDistance;
         pos.x <= m_center.x + m_loadDistance; pos.x++) {
      for (pos.y = m_center.y - m_loadDistance;
           pos.y <= m_center.y + m_loadDistance; pos.y++) {
        if (m_chunkMap.at(pos)->chunkState == ChunkState::UNGENERATED &&
            !m_chunkTerrainLoadInfoMap.count(pos)) {
          m_chunksToLoadVector.emplace_back(pos);
          m_chunkTerrainLoadInfoMap.emplace(pos,
                                            std::make_unique<ChunkLoadInfo>(pos,
                                                                            m_seed));
        }
      }
      std::sort(m_chunksToLoadVector.begin(),
                m_chunksToLoadVector.end(),
                rcmpVec2);
    }
  }
}

void World::updateChunkStructureGenList() {
  for (auto chunkKeyIter = m_chunkStructureGenInfoMap.begin();
       chunkKeyIter != m_chunkStructureGenInfoMap.end();) {
    if (chunkKeyIter->second->m_done) {
      // must erase from height map before reassigning chunkKeyIter.
      m_heightMapsMap.erase(chunkKeyIter->first);
      chunkKeyIter = m_chunkStructureGenInfoMap.erase(chunkKeyIter);
    } else {
      chunkKeyIter++;
    }
  }

  if (m_xyChanged || m_renderSet.empty()) {
    m_chunksInStructureGenRangeVector.clear();
    glm::ivec2 pos;
    for (pos.x = m_center.x - m_structureLoadDistance;
         pos.x <= m_center.x + m_structureLoadDistance; pos.x++) {
      for (pos.y = m_center.y - m_structureLoadDistance;
           pos.y <= m_center.y + m_structureLoadDistance; pos.y++) {
        if (m_chunkMap.at(pos)->chunkState == ChunkState::TERRAIN_GENERATED
            && !m_chunkStructureGenInfoMap.count(pos)) {
          m_chunksInStructureGenRangeVector.emplace_back(pos);
        }
      }
    }
    std::sort(m_chunksInStructureGenRangeVector.begin(),
              m_chunksInStructureGenRangeVector.end(),
              rcmpVec2);
  }

  for (auto posIt = m_chunksInStructureGenRangeVector.begin();
       posIt != m_chunksInStructureGenRangeVector.end();) {
    if (hasAllNeighborsInStates(*posIt, ChunkState::TERRAIN_GENERATED, ChunkState::FULLY_GENERATED)) {

      /*
 * Neighbor Chunks Array Structure
 *
 * \------------------ x
 *  \  0  3  6
 *   \  1  4  7
 *    \  2  5  8
 *     y
 */
      Chunk &chunk0 = *getChunkRawPtr({posIt->x - 1, posIt->y - 1});
      Chunk &chunk1 = *getChunkRawPtr({posIt->x - 1, posIt->y});
      Chunk &chunk2 = *getChunkRawPtr({posIt->x - 1, posIt->y + 1});
      Chunk &chunk3 = *getChunkRawPtr({posIt->x, posIt->y - 1});
      Chunk &chunk4 = *getChunkRawPtr({posIt->x, posIt->y});
      Chunk &chunk5 = *getChunkRawPtr({posIt->x, posIt->y + 1});
      Chunk &chunk6 = *getChunkRawPtr({posIt->x + 1, posIt->y - 1});
      Chunk &chunk7 = *getChunkRawPtr({posIt->x + 1, posIt->y});
      Chunk &chunk8 = *getChunkRawPtr({posIt->x + 1, posIt->y + 1});

      m_chunkStructureGenInfoMap.emplace(*posIt,
                                         std::make_unique<
                                             ChunkGenerateStructuresInfo>(chunk0,
                                                                          chunk1,
                                                                          chunk2,
                                                                          chunk3,
                                                                          chunk4,
                                                                          chunk5,
                                                                          chunk6,
                                                                          chunk7,
                                                                          chunk8,
                                                                          m_seed));

      auto
          insertPos = std::lower_bound(m_chunksReadyToGenStructuresList.begin(),
                                       m_chunksReadyToGenStructuresList.end(),
                                       *posIt,
                                       rcmpVec2);
      m_chunksReadyToGenStructuresList.insert(insertPos, *posIt);
      posIt = m_chunksInStructureGenRangeVector.erase(posIt);
    } else {
      posIt++;
    }
  }
}

void World::updateChunkMeshList() {
  // apply meshes and update map regardless of whether pos changed
  for (auto posIt = m_chunkMeshInfoMap.begin();
       posIt != m_chunkMeshInfoMap.end();) {
    if (posIt->second->m_done) {
      if (chunkExists(posIt->first)) {
        Chunk *chunk = getChunkRawPtr(posIt->first);
        posIt->second->applyMesh(chunk);
        if (!chunk->getMesh().vertices.empty()) {
          m_renderSet.insert(posIt->first);
        }
      }
      // delete from map regardless of whether chunk exists
      posIt = m_chunkMeshInfoMap.erase(posIt);
    } else {
      posIt++;
    }
  }

  if (m_xyChanged || m_renderSet.empty()) {
    m_chunksInMeshRangeVector.clear();
    // add chunks that can be meshed into vector
    glm::ivec2 pos;
    for (pos.x = m_center.x - m_renderDistance;
         pos.x <= m_center.x + m_renderDistance; pos.x++) {
      for (pos.y = m_center.y - m_renderDistance;
           pos.y <= m_center.y + m_renderDistance; pos.y++) {
        if (!chunkExists(pos)) continue; // skip non existent chunks
        Chunk *chunk = getChunkRawPtr(pos);
        if (chunk->chunkMeshState == ChunkMeshState::BUILT) {
          continue; // skip built chunks
        }
        if (chunk->chunkState != ChunkState::FULLY_GENERATED) {
          continue;
        }
        if (m_chunkMeshInfoMap.find(pos) != m_chunkMeshInfoMap.end()) {
          continue; // skip chunks already meshing
        }
        m_chunksInMeshRangeVector.emplace_back(pos);
      }
    }
    m_chunksReadyToMeshList.sort(rcmpVec2);
  }

  for (auto posIt = m_chunksInMeshRangeVector.begin();
       posIt != m_chunksInMeshRangeVector.end();) {
    if (hasAllNeighborsInState(*posIt, ChunkState::FULLY_GENERATED)) {


/*
 * Neighbor Chunks Array Structure
 *
 * \------------------ x
 *  \  0  3  6
 *   \  1  4  7
 *    \  2  5  8
 *     y
 */
      const Chunk &chunk0 = *getChunkRawPtr({posIt->x - 1, posIt->y - 1});
      const Chunk &chunk1 = *getChunkRawPtr({posIt->x - 1, posIt->y});
      const Chunk &chunk2 = *getChunkRawPtr({posIt->x - 1, posIt->y + 1});
      const Chunk &chunk3 = *getChunkRawPtr({posIt->x, posIt->y - 1});
      const Chunk &chunk4 = *getChunkRawPtr({posIt->x, posIt->y});
      const Chunk &chunk5 = *getChunkRawPtr({posIt->x, posIt->y + 1});
      const Chunk &chunk6 = *getChunkRawPtr({posIt->x + 1, posIt->y - 1});
      const Chunk &chunk7 = *getChunkRawPtr({posIt->x + 1, posIt->y});
      const Chunk &chunk8 = *getChunkRawPtr({posIt->x + 1, posIt->y + 1});

      m_chunkMeshInfoMap.emplace(*posIt,
                                 std::make_unique<ChunkMeshInfo>(chunk0,
                                                                 chunk1,
                                                                 chunk2,
                                                                 chunk3,
                                                                 chunk4,
                                                                 chunk5,
                                                                 chunk6,
                                                                 chunk7,
                                                                 chunk8));
      auto insertPos = std::lower_bound(m_chunksReadyToMeshList.begin(),
                                        m_chunksReadyToMeshList.end(),
                                        *posIt,
                                        rcmpVec2);
      m_chunksReadyToMeshList.insert(insertPos, *posIt);
      posIt = m_chunksInMeshRangeVector.erase(posIt);
    } else {
      ++posIt;
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
      it->second->unload();
      m_renderSet.erase(pos);
      it = m_chunkMap.erase(it);
    } else {
      ++it;
    }
  }
}

void World::generateChunkMeshWorker() {
  glm::ivec2 pos;
  while (m_isRunning) {
    std::unique_lock<std::mutex> lock(m_mainMutex);
    m_conditionVariable.wait(lock, [this]() {
      return !m_isRunning ||
          (m_numRunningThreads < m_numLoadingThreads &&
              !m_chunksReadyToMeshList.empty());
    });
    if (!m_isRunning) return;

    pos = m_chunksReadyToMeshList.back();
    m_chunksReadyToMeshList.pop_back();

    lock.unlock();
    m_numRunningThreads++;
    m_chunkMeshInfoMap.at(pos)->process();
    m_numRunningThreads--;
  }

}

void World::generateTerrainWorker() {
  glm::ivec2 pos;
  while (m_isRunning) {
    std::unique_lock<std::mutex> lock(m_mainMutex);
    m_conditionVariable.wait(lock, [this]() {
      return !m_isRunning ||
          (m_numRunningThreads < m_numLoadingThreads
              && !m_chunksToLoadVector.empty());
    });
    if (!m_isRunning) return;

    pos = m_chunksToLoadVector.back();
    m_chunksToLoadVector.pop_back();
    lock.unlock();
    m_numRunningThreads++;
    Scope<std::array<int, CHUNK_AREA>> heights = m_chunkTerrainLoadInfoMap.at(pos)->process();
    std::lock_guard<std::mutex> lock2(m_mainMutex);
    m_heightMapsMap.emplace(pos, std::move(heights));
    m_numRunningThreads--;
  }
}

void World::generateStructuresWorker() {
  glm::ivec2 pos;
  while (m_isRunning) {
    std::unique_lock<std::mutex> lock(m_mainMutex);
    m_conditionVariable.wait(lock, [this]() {
      return !m_isRunning ||
          (m_numRunningThreads < m_numLoadingThreads
              && !m_chunksReadyToGenStructuresList.empty());
    });
    if (!m_isRunning) return;

    pos = m_chunksReadyToGenStructuresList.back();
    m_chunksReadyToGenStructuresList.pop_back();
    const std::array<int, CHUNK_AREA> &heightMap = *m_heightMapsMap.at(pos).get();
    lock.unlock();
    m_numRunningThreads++;
    m_chunkStructureGenInfoMap.at(pos)->process(heightMap);
    m_numRunningThreads--;
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

      setBlockFromWorldPosition({blockPos.x, blockPos.y, blockPos.z},
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
      setBlockFromWorldPosition(lastAirBlockPos,
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
  if (ImGui::SliderInt("Render Distance", &renderDistance, 1, 32)) {
    setRenderDistance(renderDistance);
  }

  bool useAmbientOcclusion = Config::getUseAmbientOcclusion();
  if (ImGui::Checkbox("Ambient Occlusion", &useAmbientOcclusion)) {
    Config::setUseAmbientOcclusion(useAmbientOcclusion);
    chunkRenderer.updateShaderUniforms();
  }

  ImGui::Text("lastRayCastBlockPos: %d, %d, %d", m_lastRayCastBlockPos.x,
              m_lastRayCastBlockPos.y, m_lastRayCastBlockPos.z);

  ImGui::Text("Running Threads: %d", static_cast<int>(m_numRunningThreads));

  player.renderDebugGui();

  ImGui::End();
}

void World::saveData() {
  m_worldSave.saveData();
}

bool World::hasAllNeighborsInState(const glm::ivec2 &pos, ChunkState state) {
  return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(),
                     NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                     [&](glm::ivec2 offset) {
                       auto neighborPos =
                           glm::ivec2(pos.x + offset.x, pos.y + offset.y);
                       return chunkExists(neighborPos) &&
                           m_chunkMap.at(neighborPos)->chunkState == state;
                     });
}

void World::setBlockFromWorldPosition(glm::ivec3 pos, Block block) {
  // todo fix args here
  auto chunkPos = chunkPosFromWorldPos(pos.x, pos.y);
  Chunk *chunk = getChunkRawPtr(chunkPos);
  int chunkX = Utils::positiveModulo(pos.x, CHUNK_WIDTH);
  int chunkY = Utils::positiveModulo(pos.y, CHUNK_WIDTH);

  chunk->setBlock(chunkX, chunkY, pos.z, block);
//    handleChunkUpdates(chunk, chunkKey, chunkX, chunkY);
}

Block World::getBlockFromWorldPosition(glm::ivec3 position) {
  auto chunkPos = chunkPosFromWorldPos(position.x, position.y);
  Chunk *chunk = getChunkRawPtr(chunkPos);
  int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
  int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);
  return chunk->getBlock(chunkX, chunkY, position.z);
}
void World::setRenderDistance(int renderDistance) {
  m_renderDistance = renderDistance;
  m_loadDistance = m_renderDistance + 2;
  m_structureLoadDistance = m_renderDistance + 1;
  m_unloadDistance = m_renderDistance + 4;
}
bool World::hasAllNeighborsInStates(const glm::ivec2 &pos,
                                    ChunkState state1,
                                    ChunkState state2) {
  return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(),
                     NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                     [&](glm::ivec2 offset) {
                       auto neighborPos =
                           glm::ivec2(pos.x + offset.x, pos.y + offset.y);
                       return chunkExists(neighborPos) &&
                           (m_chunkMap.at(neighborPos)->chunkState == state1
                               || m_chunkMap.at(neighborPos)->chunkState
                                   == state2);
                        });

}


