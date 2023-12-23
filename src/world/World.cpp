//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.hpp"
#include "generation/TerrainGenerator.hpp"
#include "../utils/Timer.hpp"
#include "../Config.hpp"
#include "block/BlockDB.hpp"
#include "../input/Mouse.hpp"
#include <algorithm>


World::World(Renderer &renderer, int seed, const std::string &savePath) : m_worldSave(savePath),
                                                                          m_renderer(renderer),
                                                                          chunkRenderer(
                                                                                  player.camera),
                                                                          m_terrainGenerator(seed),
                                                                          m_center(INT_MAX),
                                                                          m_numRunningThreads(0),
                                                                          m_numLoadingThreads(
                                                                                  std::thread::hardware_concurrency()) {
    rcmpChunkKey = std::bind(&World::rcmpChunkKey_impl, this, std::placeholders::_1,
                             std::placeholders::_2);

    BlockDB::loadData("../resources/blocks/");

    for (int i = 0; i < std::thread::hardware_concurrency(); i++) {
//        m_chunkMeshThreads.emplace_back(&World::loadChunksWorker, this);
        m_chunkMeshThreads.emplace_back(&World::updateChunkMeshes, this);
    }

    m_chunkLoadThreads.emplace_back([&]() {
        while (m_isRunning) {
            loadChunks();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

World::~World() {
    std::cout << "World destructor\n";
    m_isRunning = false;
    for (auto &thread: m_chunkLoadThreads) {
        thread.join();
    }
    saveData();
}

void sortChunksByDistance(std::vector<Ref<Chunk>> &chunks, const glm::vec3 &playerPos) {
    std::sort(chunks.begin(), chunks.end(), [&playerPos](const Ref<Chunk> &a, const Ref<Chunk> &b) {
        return (glm::distance(a.get()->location(), playerPos) >
                glm::distance(b.get()->location(), playerPos));
    });
}

void World::render() {
    chunkRenderer.start();
//    std::vector<Ref<Chunk>> chunksToRender;
//    for (auto &chunkEntry: chunkMap) {
//        if (chunkEntry.second->chunkMeshState == ChunkMeshState::BUILT) {
//            chunksToRender.push_back(chunkEntry.second);
//        }
//    }
//
//    sortChunksByDistance(chunksToRender, player.getPosition());
//
//    for (auto &chunk: chunksToRender) {
//        chunkRenderer.render(chunk);
//    }

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

    if (m_xyChanged) {
        unloadChunks();
    }

    {
        std::lock_guard<std::mutex> lock(m_mainMutex);
        for (auto chunkKeyIter = m_chunkMeshInfoMap.begin();
             chunkKeyIter != m_chunkMeshInfoMap.end();) {
            if (chunkKeyIter->second->m_done) {
                if (m_chunkManager.chunkExists(chunkKeyIter->first)) {
                    const Ref<Chunk> &chunk = m_chunkManager.getChunk(chunkKeyIter->first);
                    chunkKeyIter->second->applyMesh(chunk);
                    if (!chunk->getMesh().vertices.empty()) {
//                        m_renderVector.push_back(chunkKeyIter->first);
                        m_renderSet.insert(chunkKeyIter->first);
                    }
                }
                chunkKeyIter = m_chunkMeshInfoMap.erase(chunkKeyIter);
            } else {
                ++chunkKeyIter;
            }
        }
        if (m_xyChanged) {
            m_chunksInMeshRangeVector.clear();
            // add chunks that can be meshed into vector
            auto center = player.getChunkKeyPos();
            for (int x = center.x - m_renderDistance; x <= m_renderDistance; x++) {
                for (int y = center.y - m_renderDistance; y <= m_renderDistance; y++) {
                    ChunkKey chunkKey = {playerChunkKey.x + x, playerChunkKey.y + y};
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

            m_chunksReadyToMeshList.sort(rcmpChunkKey);
        }

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
//                std::cout << "ready to mesh size: " << m_chunksReadyToMeshList.size() << std::endl;
//                m_chunksReadyToMeshSet.insert(*chunkKeyIter);
                chunkKeyIter = m_chunksInMeshRangeVector.erase(chunkKeyIter);
            } else {
                ++chunkKeyIter;
            }
        }

        m_conditionVariable.notify_all();
    }


    castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});

    // if have chunks to remesh, lock
    if (m_chunkManager.hasChunksToRemesh()) {
        std::lock_guard<std::mutex> lock(m_mainMutex);
        m_chunkManager.remeshChunksToRemesh();
    }

}

void World::updateChunkLoadList() {


}


void World::loadChunks() {
    while (true) {
        std::cout << "load chnks\n";

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
            it = chunkMap.erase(it);
            m_renderSet.erase(chunkKey);
        } else {
            ++it;
        }
    }
}

void World::updateChunkMeshes() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainMutex);
        m_conditionVariable.wait(lock, [this]() {
            return !m_isRunning || (m_numRunningThreads < 4 && !m_chunksReadyToMeshList.empty());
        });
        if (!m_isRunning) return;

        ChunkKey chunkKey = m_chunksReadyToMeshList.back();
        m_chunksReadyToMeshList.pop_back();

        lock.unlock();
        m_numRunningThreads++;
        m_chunkMeshInfoMap.at(chunkKey)->process();
        m_numRunningThreads--;
    }

}

void World::loadChunksWorker() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mainMutex);
        m_conditionVariable.wait(lock, [this]() {
            return !m_isRunning || (m_numRunningThreads < m_numLoadingThreads && !m_chunksToLoadVector.empty());
        });
        if (!m_isRunning) return;

        ChunkKey chunkKey = m_chunksToLoadVector.back();
        m_chunksToLoadVector.pop_back();

        lock.unlock();
        m_numRunningThreads++;
        m_chunkLoadInfoMap.at(chunkKey)->process();
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

int World::getRenderDistance() const {
    return m_renderDistance;
}

void World::setRenderDistance(int renderDistance) {
    m_renderDistance = renderDistance;
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

