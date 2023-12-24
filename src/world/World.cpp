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
          chunkRenderer(player.camera), m_terrainGenerator(seed),
          m_center(INT_MAX), m_numRunningThreads(0),
          m_numLoadingThreads(std::thread::hardware_concurrency()),
          m_seed(seed) {
    rcmpChunkKey = [this](auto &&PH1, auto &&PH2) {
        return rcmpChunkKey_impl(std::forward<decltype(PH1)>(PH1),
                                 std::forward<decltype(PH2)>(PH2));
    };
    const size_t loadVectorSize =
            ((size_t) (m_renderDistance + 2) * 2 + 1) * ((size_t) (m_renderDistance + 2) * 2 + 1);
    m_chunksToLoadVector.reserve(loadVectorSize);

    BlockDB::loadData("../resources/blocks/");

    for (int i = 0; i < m_numLoadingThreads; i++) {
        m_chunkLoadThreads.emplace_back(&World::loadChunksWorker, this);
        m_chunkLoadThreads.emplace_back(&World::updateChunkMeshes, this);
    }
}

World::~World() {
    std::cout << "World destructor\n";
    m_isRunning = false;
    for (auto &thread: m_chunkLoadThreads) {
        thread.join();
    }
    saveData();
}

void World::render() {
    chunkRenderer.start();

    for (auto &chunkKey: m_renderSet) {
//        const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKey);
        if (chunkExists(chunkKey)) {
            chunkRenderer.render(*m_chunkMap.at(chunkKey));
        }
    }
//        chunkRenderer.render(chunk);
//    }

    // render block break and outline if a block is being aimed at
    if (static_cast<const glm::vec3>(m_lastRayCastBlockPos) != NULL_VECTOR) {
        m_renderer.renderBlockOutline(player.camera, m_lastRayCastBlockPos);
        m_renderer.renderBlockBreak(player.camera, m_lastRayCastBlockPos, player.blockBreakStage);
    }
    m_renderer.renderCrossHair();
    renderDebugGui();
}

void World::update() {
//    std::cout << "mesh ready list size:       " << m_chunksReadyToMeshList.size() << std::endl;
//    std::cout << "render set size:            " << m_renderSet.size() << std::endl;
//    std::cout << "in mesh range vector size:  " << m_chunksInMeshRangeVector.size() << std::endl;
//    std::cout << "mesh info map size:         " << m_chunkMeshInfoMap.size() << std::endl;
//    std::cout << "chunks to load vector size: " << m_chunksToLoadVector.size() << std::endl;
//    std::cout << "load info map size:         " << m_chunkTerrainLoadInfoMap.size() << std::endl;
//    std::cout << "\n";

    m_xyChanged = false;

    // update center and whether position changed
    ChunkKey playerChunkKey = player.getChunkKeyPos();
    auto newCenter = glm::ivec2(playerChunkKey.x, playerChunkKey.y);
    if (newCenter != m_center) {
        m_xyChanged = true;
        m_center = newCenter;
    }

    if (m_xyChanged) {
        // create chunks in range (not terrain generated at this point)
        int loadDistance = m_renderDistance + 2;
        for (int x = m_center.x - loadDistance; x <= m_center.x + loadDistance; x++) {
            for (int y = m_center.y - loadDistance; y <= m_center.y + loadDistance; y++) {
                ChunkKey chunkKey = {x, y};
                if (chunkExists(chunkKey)) continue; // skip non existent chunks
                m_chunkMap.emplace(chunkKey, std::make_unique<Chunk>(chunkKey));
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


    std::lock_guard<std::mutex> lock(m_mainMutex);
    updateChunkLoadList();
    updateChunkMeshList();
    m_conditionVariable.notify_all();
}

void World::updateChunkLoadList() {
    {
        for (auto chunkKeyIter = m_chunkTerrainLoadInfoMap.begin();
             chunkKeyIter != m_chunkTerrainLoadInfoMap.end();) {
            if (chunkKeyIter->second->m_done) {
                if (chunkExists(chunkKeyIter->first)) {
                    chunkKeyIter->second->applyTerrain(getChunkRawPtr(chunkKeyIter->first));
                }
                chunkKeyIter = m_chunkTerrainLoadInfoMap.erase(chunkKeyIter);
            } else {
                chunkKeyIter++;
            }
        }
    }
    if (m_xyChanged) {
        {
            Timer t("load vector", false);
            int loadDistance = m_renderDistance + 2;
            ChunkKey chunkKey{};
            for (chunkKey.x = m_center.x - loadDistance;
                 chunkKey.x <= m_center.x + loadDistance; chunkKey.x++) {
                for (chunkKey.y = m_center.y - loadDistance;
                     chunkKey.y <= m_center.y + loadDistance; chunkKey.y++) {
                    if (m_chunkMap.at(chunkKey)->chunkState != ChunkState::FULLY_GENERATED &&
                        !m_chunkTerrainLoadInfoMap.count(chunkKey)) {
                        m_chunksToLoadVector.emplace_back(chunkKey);
                        m_chunkTerrainLoadInfoMap.emplace(chunkKey,
                                                          std::make_unique<ChunkLoadInfo>(chunkKey,
                                                                                          m_seed));
                    }
                }
                std::sort(m_chunksToLoadVector.begin(), m_chunksToLoadVector.end(), rcmpChunkKey);
            }

            auto dur = t.stop();
            if (dur > 0) {
                std::cout << "load vector: " << dur << std::endl;
            }
        }
    }
}

void World::updateChunkMeshList() {
    // apply meshes regardless of whether pos changed
    for (auto chunkKeyIter = m_chunkMeshInfoMap.begin();
         chunkKeyIter != m_chunkMeshInfoMap.end();) {
        if (chunkKeyIter->second->m_done) {
            if (chunkExists(chunkKeyIter->first)) {
                Chunk *chunk = getChunkRawPtr(chunkKeyIter->first);
                chunkKeyIter->second->applyMesh(chunk);
                if (!chunk->getMesh().vertices.empty()) {
                    m_renderSet.insert(chunkKeyIter->first);
                }
            }
            chunkKeyIter = m_chunkMeshInfoMap.erase(chunkKeyIter);
        } else {
            chunkKeyIter++;
        }
    }
//    if (m_xyChanged) {
    auto k = ChunkKey{-448 / CHUNK_WIDTH, 128 / CHUNK_WIDTH};
    m_chunksInMeshRangeVector.clear();
    // add chunks that can be meshed into vector
    int x, y;
    ChunkKey chunkKey{};
    for (x = m_center.x - m_renderDistance; x <= m_center.x + m_renderDistance; x++) {
        for (y = m_center.y - m_renderDistance; y <= m_center.y + m_renderDistance; y++) {
            chunkKey = {x, y};
            if (!chunkExists(chunkKey)) continue; // skip non existent chunks
            Chunk *chunk = getChunkRawPtr(chunkKey);
            if (chunk->chunkState != ChunkState::FULLY_GENERATED) {
                continue; // skip ineligible chunks
            }
            if (chunk->chunkMeshState == ChunkMeshState::BUILT) {
                continue; // skip built chunks
            }
            if (m_chunkMeshInfoMap.find(chunkKey) != m_chunkMeshInfoMap.end()) {
                continue; // skip chunks already meshing
            }
            m_chunksInMeshRangeVector.emplace_back(chunkKey);
        }
    }
    m_chunksReadyToMeshList.sort(rcmpChunkKey);
//    }

    glm::ivec2 offset;
    int i;
    Chunk *chunks[9];
    for (auto chunkKeyIter = m_chunksInMeshRangeVector.begin();
         chunkKeyIter != m_chunksInMeshRangeVector.end();) {
        bool hasAllNeighbors = hasAllNeighborsFullyGenerated(*chunkKeyIter);
        if (hasAllNeighbors) {
            for (i = 0; i < 9; i++) {
                offset = NEIGHBOR_ARRAY_OFFSETS[i];
                chunks[i] = getChunkRawPtr(
                        {offset.x + chunkKeyIter->x, offset.y + chunkKeyIter->y});
            }
            m_chunkMeshInfoMap.emplace(*chunkKeyIter, std::make_unique<ChunkMeshInfo>(chunks));
            auto pos = std::lower_bound(m_chunksReadyToMeshList.begin(),
                                        m_chunksReadyToMeshList.end(), *chunkKeyIter,
                                        rcmpChunkKey);
            m_chunksReadyToMeshList.insert(pos, *chunkKeyIter);
            chunkKeyIter = m_chunksInMeshRangeVector.erase(chunkKeyIter);
        } else {
            if (*chunkKeyIter == k) {
                std::cout << "not all neighbors\n";
            }
            ++chunkKeyIter;
        }
    }
}

void World::unloadChunks() {
    std::unordered_set<ChunkKey> chunksToUnload;
    int unloadDistanceChunks = m_renderDistance + 4;
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    for (auto it = m_chunkMap.begin(); it != m_chunkMap.end();) {
        ChunkKey chunkKey = it->first;
        if (chunkKey.x < playerChunkKeyPos.x - unloadDistanceChunks ||
            chunkKey.x > playerChunkKeyPos.x + unloadDistanceChunks ||
            chunkKey.y < playerChunkKeyPos.y - unloadDistanceChunks ||
            chunkKey.y > playerChunkKeyPos.y + unloadDistanceChunks) {
            it->second->unload();
            m_renderSet.erase(chunkKey);
            it = m_chunkMap.erase(it);
        } else {
            ++it;
        }
    }
}

void World::updateChunkMeshes() {
    ChunkKey chunkKey{};
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainMutex);
        m_conditionVariable.wait(lock, [this]() {
            return !m_isRunning ||
                   (m_numRunningThreads < m_numLoadingThreads &&
                    !m_chunksReadyToMeshList.empty());
        });
        if (!m_isRunning) return;
        chunkKey = m_chunksReadyToMeshList.back();
        m_chunksReadyToMeshList.pop_back();

        lock.unlock();
        m_numRunningThreads++;
        m_chunkMeshInfoMap.at(chunkKey)->process();
        m_numRunningThreads--;
    }

}

void World::loadChunksWorker() {
    ChunkKey chunkKey{};
//    static int count = 0;
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainMutex);
        m_conditionVariable.wait(lock, [this]() {
            return !m_isRunning ||
                   (m_numRunningThreads < m_numLoadingThreads && !m_chunksToLoadVector.empty());
        });
        if (!m_isRunning) return;
//        count++;
//        std::cout << "load worker count: " << count << ", " << m_numRunningThreads << std::endl;
        chunkKey = m_chunksToLoadVector.back();
        m_chunksToLoadVector.pop_back();
        lock.unlock();
        m_numRunningThreads++;
        m_chunkTerrainLoadInfoMap.at(chunkKey)->process();
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
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
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

            setBlockFromWorldPosition({blockPos.x, blockPos.y, blockPos.z}, Block::AIR);
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
            setBlockFromWorldPosition(lastAirBlockPos, Block(player.inventory.getHeldItem()));
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

    ImGui::SliderInt("Render Distance", &m_renderDistance, 1, 32);

    bool useAmbientOcclusion = Config::getUseAmbientOcclusion();
    if (ImGui::Checkbox("Ambient Occlusion", &useAmbientOcclusion)) {
        Config::setUseAmbientOcclusion(useAmbientOcclusion);
        chunkRenderer.updateShaderUniforms();
    }

    ImGui::Text("lastRayCastBlockPos: %d, %d, %d", m_lastRayCastBlockPos.x,
                m_lastRayCastBlockPos.y, m_lastRayCastBlockPos.z);

    player.renderDebugGui();

    ImGui::End();
}

void World::saveData() {
    m_worldSave.saveData();
}


bool World::hasAllNeighbors(ChunkKey chunkKey) {
    return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(), NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                       [&](glm::ivec2 offset) {
                           return chunkExists(
                                   ChunkKey{chunkKey.x + offset.x, chunkKey.y + offset.y});
                       });
}

bool World::hasAllNeighborsFullyGenerated(ChunkKey chunkKey) {
    return std::all_of(NEIGHBOR_CHUNK_KEY_OFFSETS.begin(), NEIGHBOR_CHUNK_KEY_OFFSETS.end(),
                       [&](glm::ivec2 offset) {
                           auto neighborChunkKey = ChunkKey{chunkKey.x + offset.x,
                                                            chunkKey.y + offset.y};
                           return chunkExists(neighborChunkKey) &&
                                  m_chunkMap.at(neighborChunkKey)->chunkState ==
                                  ChunkState::FULLY_GENERATED;
                       });
}

void World::setBlockFromWorldPosition(glm::ivec3 position, Block block) {
    auto chunkKey = ChunkManager::getChunkKeyByWorldLocation(position.x, position.y);
    Chunk *chunk = getChunkRawPtr(chunkKey);
    int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
    int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);

    chunk->setBlock(chunkX, chunkY, position.z, block);
//    handleChunkUpdates(chunk, chunkKey, chunkX, chunkY);
}

Block World::getBlockFromWorldPosition(glm::ivec3 position) {
    auto chunkKey = getChunkKeyByWorldLocation(position.x, position.y);
    Chunk *chunk = getChunkRawPtr(chunkKey);
    int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
    int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);
    return chunk->getBlock(chunkX, chunkY, position.z);
}