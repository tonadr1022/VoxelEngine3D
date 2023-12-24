//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.hpp"
#include "../Config.hpp"
#include "block/BlockDB.hpp"
#include "../input/Mouse.hpp"
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

    BlockDB::loadData("../resources/blocks/");

    for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
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
        const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKey);
        chunkRenderer.render(chunk);
    }

    // render block break and outline if a block is being aimed at
    if (static_cast<const glm::vec3>(m_lastRayCastBlockPos) != NULL_VECTOR) {
        m_renderer.renderBlockOutline(player.camera, m_lastRayCastBlockPos);
        m_renderer.renderBlockBreak(player.camera, m_lastRayCastBlockPos, player.blockBreakStage);
    }
    m_renderer.renderCrossHair();
    renderDebugGui();
}

void World::update() {
    m_xyChanged = false;
    ChunkKey playerChunkKey = player.getChunkKeyPos();
    auto newCenter = glm::ivec3(playerChunkKey.x, playerChunkKey.y, 0);

    if (newCenter != m_center) {
        m_xyChanged = true;
        m_center = newCenter;
    }

    {
        Timer timer("update", false);
        ChunkMap &chunkMap = m_chunkManager.getChunkMap();
        std::cout << "chunkMap size: " << chunkMap.size() << std::endl;

        // create chunks in range (not terrain generated at this point)
        int loadDistance = m_renderDistance + 2;
        glm::ivec2 pos = {m_center.x, m_center.y};
        for (int x = pos.x - loadDistance; x <= pos.x + loadDistance; x++) {
            for (int y = pos.y - loadDistance; y <= pos.x + loadDistance; y++) {
                ChunkKey chunkKey = {x, y};
                if (chunkMap.find(chunkKey) != chunkMap.end()) continue; // skip non existent chunks
//                chunkMap.emplace(chunkKey, std::make_shared<Chunk>(chunkKey));
                {
//                    Timer t("emplace");
                    chunkMap.emplace(chunkKey, std::make_shared<Chunk>(
                            glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH)));
                }
            }
        }

        float dur = timer.stop();
        if (dur > 30.0f) {
            std::cout << "create chunks took " << dur << " milliseconds" << std::endl;
        }
    }

    if (m_xyChanged) {
        unloadChunks();
    }

    castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});

    // if have chunks to remesh, lock
    if (m_chunkManager.hasChunksToRemesh()) {
        std::lock_guard<std::mutex> lock(m_mainMutex);
        m_chunkManager.remeshChunksToRemesh();
    }

    {
        std::lock_guard<std::mutex> lock(m_mainMutex);
        updateChunkLoadList();
        updateChunkMeshList();
        m_conditionVariable.notify_all();
    }


}

void World::updateChunkLoadList() {
//    Timer t("updateChunkLoadList", false);
    for (auto chunkKeyIter = m_chunkTerrainLoadInfoMap.begin();
         chunkKeyIter != m_chunkTerrainLoadInfoMap.end();) {
        if (chunkKeyIter->second->m_done) {
            if (m_chunkManager.chunkExists(chunkKeyIter->first)) {
                const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKeyIter->first);
                chunkKeyIter->second->applyTerrain(chunk);
                chunkKeyIter = m_chunkTerrainLoadInfoMap.erase(chunkKeyIter);
            }
        } else {
            chunkKeyIter++;
        }
    }
//    if (m_xyChanged) {
    int loadDistance = m_renderDistance + 2;
    for (int x = m_center.x - loadDistance; x <= m_center.x + loadDistance; x++) {
        for (int y = m_center.y - loadDistance; y <= m_center.y + loadDistance; y++) {
            ChunkKey chunkKey = {x, y};
            if (!m_chunkManager.chunkExists(chunkKey) ||
                m_chunkManager.getChunk(chunkKey)->chunkState ==
                ChunkState::FULLY_GENERATED ||
                m_chunkTerrainLoadInfoMap.find(chunkKey) != m_chunkTerrainLoadInfoMap.end())
                continue; // skip non existent chunks, ineligible chunks, and chunks already loading
            m_chunksToLoadVector.emplace_back(chunkKey);
            m_chunkTerrainLoadInfoMap.emplace(chunkKey,
                                              std::make_unique<ChunkLoadInfo>(chunkKey,
                                                                              m_seed));
        }
//        }
    }
//    if (!m_chunksToLoadVector.empty()) {
//        std::cout << "chunks to load: " << m_chunksToLoadVector.size() << std::endl;
//        std::sort(m_chunksToLoadVector.begin(), m_chunksToLoadVector.end(), rcmpChunkKey);
//    }
//    float time = t.stop();
//    if (time > 1.0f) {
//        std::cout << "updateChunkLoadList took " << time << " milliseconds" << std::endl;
//    }
}

void World::updateChunkMeshList() {
//    Timer t("updateChunkMeshList", false);
    for (auto chunkKeyIter = m_chunkMeshInfoMap.begin();
         chunkKeyIter != m_chunkMeshInfoMap.end();) {
        if (chunkKeyIter->second->m_done) {
            if (m_chunkManager.chunkExists(chunkKeyIter->first)) {
                const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKeyIter->first);
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
    m_chunksInMeshRangeVector.clear();
    // add chunks that can be meshed into vector
    auto center = player.getChunkKeyPos();
    for (int x = center.x - m_renderDistance; x <= m_renderDistance; x++) {
        for (int y = center.y - m_renderDistance; y <= m_renderDistance; y++) {
            ChunkKey chunkKey = {center.x + x, center.y + y};
            if (!m_chunkManager.chunkExists(chunkKey)) continue; // skip non existent chunks
            const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKey);
            if (chunk->chunkState != ChunkState::FULLY_GENERATED)
                continue; // skip ineligible chunks
            if (chunk->chunkMeshState == ChunkMeshState::BUILT)
                continue; // skip built chunks
            if (m_chunkMeshInfoMap.find(chunkKey) != m_chunkMeshInfoMap.end())
                continue; // skip chunks already meshing
            m_chunksInMeshRangeVector.emplace_back(chunkKey);
        }
    }
//        m_chunksReadyToMeshList.sort(rcmpChunkKey);

    {
        glm::ivec2 offset;
        int i;
        Ref<Chunk> chunks[9];
        for (auto chunkKeyIter = m_chunksInMeshRangeVector.begin();
             chunkKeyIter != m_chunksInMeshRangeVector.end();) {
            bool hasAllNeighbors = m_chunkManager.hasAllNeighborsFullyGenerated(*chunkKeyIter);
            if (hasAllNeighbors) {
                for (i = 0; i < 9; i++) {
                    offset = ChunkManager::NEIGHBOR_ARRAY_OFFSETS[i];
                    chunks[i] = m_chunkManager.getChunk({offset.x + chunkKeyIter->x,
                                                         offset.y + chunkKeyIter->y});
                }
                m_chunkMeshInfoMap.emplace(*chunkKeyIter, std::make_unique<ChunkMeshInfo>(chunks));
                auto pos = std::lower_bound(m_chunksReadyToMeshList.begin(),
                                            m_chunksReadyToMeshList.end(), *chunkKeyIter,
                                            rcmpChunkKey);
                m_chunksReadyToMeshList.insert(pos, *chunkKeyIter);
                chunkKeyIter = m_chunksInMeshRangeVector.erase(chunkKeyIter);
            } else {
                ++chunkKeyIter;
            }
        }
//        float duration = timer.stop();
//        if (duration > 1.0f) {
//            std::cout << "f took " << duration << " milliseconds" << std::endl;
//        }
    }
//    float time = t.stop();
//    if (time > 1.0f) {
//        std::cout << "updateChunkMeshList took " << time << " milliseconds" << std::endl;
//    }
}


void World::loadChunks() {
    while (true) {
        int firstPassLoadDistance = m_renderDistance + 2;
        int secondPassLoadDistance = firstPassLoadDistance - 1;

        ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
        ChunkMap &chunkMap = m_chunkManager.getChunkMap();
        int numTerrainBuilt = 0;
        for (int i = 1; i <= firstPassLoadDistance; i++) {
            int minX = playerChunkKeyPos.x - i;
            int maxX = playerChunkKeyPos.x + i;
            int minY = playerChunkKeyPos.y - i;
            int maxY = playerChunkKeyPos.y + i;
            for (int chunkX = minX; chunkX <= maxX; chunkX++) {
                for (int chunkY = minY; chunkY <= maxY && numTerrainBuilt < 10; chunkY++) {
                    ChunkKey chunkKey = {chunkX, chunkY};
                    std::unique_lock<std::mutex> lock(m_mainMutex);
                    auto it = m_chunkManager.getChunkMap().find(chunkKey);
                    if (it == m_chunkManager.getChunkMap().end()) {
                        const Ref<Chunk> chunk = std::make_shared<Chunk>(
                                glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                        m_chunkManager.getChunkMap().emplace(chunkKey, chunk);
                        lock.unlock();
                        m_terrainGenerator.generateTerrainFor(chunk);
                        numTerrainBuilt++;
                    } else {
                        lock.unlock();
                    }
                }
            }
        }

        if (numTerrainBuilt == 0) return;
        numTerrainBuilt = 0;
        for (int i = 1; i <= secondPassLoadDistance; i++) {
            int minX = playerChunkKeyPos.x - i;
            int maxX = playerChunkKeyPos.x + i;
            int minY = playerChunkKeyPos.y - i;
            int maxY = playerChunkKeyPos.y + i;
            for (int chunkX = minX; chunkX <= maxX; chunkX++) {
                for (int chunkY = minY; chunkY <= maxY && numTerrainBuilt < 10; chunkY++) {
                    ChunkKey chunkKey = {chunkX, chunkY};
                    std::lock_guard<std::mutex> lock(m_mainMutex);
                    auto it = chunkMap.find(chunkKey);
                    if (it != chunkMap.end() &&
                        it->second->chunkState == ChunkState::TERRAIN_GENERATED &&
                        m_chunkManager.hasAllNeighbors(chunkKey)) {
                        m_terrainGenerator.generateStructuresFor(m_chunkManager, it->second);
                        numTerrainBuilt++;
                    }
                }
            }
        }
    }
}

void World::unloadChunks() {
    std::unordered_set<ChunkKey> chunksToUnload;
    int unloadDistanceChunks = m_renderDistance + 3;
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    ChunkMap &chunkMap = m_chunkManager.getChunkMap();
    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        ChunkKey chunkKey = it->first;
        const Ref<Chunk> &chunk = it->second;
        if (chunkKey.x < playerChunkKeyPos.x - unloadDistanceChunks ||
            chunkKey.x > playerChunkKeyPos.x + unloadDistanceChunks ||
            chunkKey.y < playerChunkKeyPos.y - unloadDistanceChunks ||
            chunkKey.y > playerChunkKeyPos.y + unloadDistanceChunks) {
            chunk->unload();
            std::cout << it->first.x << ", " << it->first.y << " unloaded\n";
            it = chunkMap.erase(it);
            std::cout << "new it: " << it->first.x << ", " << it->first.y << std::endl;
            m_renderSet.erase(chunkKey);
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
                   (m_numRunningThreads < m_numLoadingThreads && !m_chunksReadyToMeshList.empty());
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
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainMutex);
        m_conditionVariable.wait(lock, [this]() {
            return !m_isRunning ||
                   (m_numRunningThreads < m_numLoadingThreads && !m_chunksToLoadVector.empty());
        });
        if (!m_isRunning) return;

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
            Block block = m_chunkManager.getBlock(blockPos);
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

            m_chunkManager.setBlock({blockPos.x, blockPos.y, blockPos.z}, Block::AIR);
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
            m_chunkManager.setBlock(lastAirBlockPos, Block(player.inventory.getHeldItem()));
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

