//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.hpp"
#include "../resources/ResourceManager.hpp"
#include "generation/TerrainGenerator.hpp"

World::World(GLFWwindow *window, Renderer &renderer) : window(window), renderer(renderer),
                                                       chunkRenderer(player.camera) {
    auto start = std::chrono::high_resolution_clock::now();

    updateChunks();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Chunk generation took: " << duration.count() << "milliseconds"
              << std::endl;

    for (int i = 0; i < 1; i++) {
        m_chunkLoadThreads.emplace_back([&]() {
            while (m_isRunning) {
                updateChunks();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
}

void World::render() {
    chunkRenderer.start();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto &chunkEntry: chunkMap) {
        if (chunkEntry.second->chunkMeshState == ChunkMeshState::BUILT) {
            chunkRenderer.render(chunkEntry.second);
        }
    }

    // render block break and outline if a block is being aimed at
    if (static_cast<const glm::vec3>(lastRayCastBlockPos) != NULL_VECTOR) {
        renderer.renderBlockOutline(player.camera, lastRayCastBlockPos);
        renderer.renderBlockBreak(player.camera, lastRayCastBlockPos, player.blockBreakStage);
    }
}

void World::update() {
    unloadChunks();
    castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});
    chunkManager.remeshChunksToRemesh();
}


void World::updateChunks() {
    loadChunks();
    updateChunkMeshes();
}

void World::loadChunks() {
//    auto start = std::chrono::high_resolution_clock::now();

    int firstPassLoadDistance = m_renderDistance + 2;
    int secondPassLoadDistance = firstPassLoadDistance - 1;

    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    ChunkMap &chunkMap = chunkManager.getChunkMap();

//// First pass
//    for (int i = 1; i <= firstPassLoadDistance; i++) {
//        int minX = playerChunkKeyPos.x - i;
//        int maxX = playerChunkKeyPos.x + i;
//        int minY = playerChunkKeyPos.y - i;
//        int maxY = playerChunkKeyPos.y + i;
//
//        for (int chunkX = minX; chunkX <= maxX; chunkX++) {
//            for (int chunkY = minY; chunkY <= maxY; chunkY++) {
//                ChunkKey chunkKey = {chunkX, chunkY};
//                {
//                    std::unique_lock<std::mutex> lock(m_mainMutex);
//                    if (chunkMap.find(chunkKey) == chunkMap.end()) {
//                        const Ref<Chunk> &chunk = std::make_shared<Chunk>(
//                                glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
//                        TerrainGenerator::generateTerrainFor(chunk);
//                        chunkMap.emplace(chunkKey, chunk);
//                    }
//                }
//                std::this_thread::sleep_for(std::chrono::microseconds(1));
//            }
//        }
//    }
//
//    // Second pass for structure generation
//    for (int i = 1; i <= secondPassLoadDistance; i++) {
//        int minX = playerChunkKeyPos.x - i;
//        int maxX = playerChunkKeyPos.x + i;
//        int minY = playerChunkKeyPos.y - i;
//        int maxY = playerChunkKeyPos.y + i;
//
//        for (int chunkX = minX; chunkX <= maxX; chunkX++) {
//            for (int chunkY = minY; chunkY <= maxY; chunkY++) {
//                ChunkKey chunkKey = {chunkX, chunkY};
//                {
//                    std::unique_lock<std::mutex> lock(m_mainMutex);
//                    if (chunkMap.find(chunkKey) != chunkMap.end()) {
//                        const Ref<Chunk> &chunk = chunkMap.at(chunkKey);
//                        if (chunk->chunkState == ChunkState::TERRAIN_GENERATED &&
//                            chunkManager.hasAllNeighbors(chunkKey)) {
//                            TerrainGenerator::generateStructuresFor(chunkManager, chunk);
//                        }
//                    }
//                }
//                std::this_thread::sleep_for(std::chrono::microseconds(1));
//            }
//        }
//    }
    // First pass
    for (int i = 1; i <= firstPassLoadDistance; i++) {
        int minX = playerChunkKeyPos.x - i;
        int maxX = playerChunkKeyPos.x + i;
        int minY = playerChunkKeyPos.y - i;
        int maxY = playerChunkKeyPos.y + i;

        for (int chunkX = minX; chunkX <= maxX; chunkX++) {
            for (int chunkY = minY; chunkY <= maxY; chunkY++) {
                ChunkKey chunkKey = {chunkX, chunkY};
                {
                    std::unique_lock<std::mutex> lock(m_mainMutex);
                    if (chunkMap.find(chunkKey) == chunkMap.end()) {
                        const Ref<Chunk> &chunk = std::make_shared<Chunk>(
                                glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                        TerrainGenerator::generateTerrainFor(chunk);
                        chunkMap.emplace(chunkKey, chunk);
                    } else if (i <= secondPassLoadDistance && chunkMap.at(chunkKey)->chunkState == ChunkState::TERRAIN_GENERATED &&
                               chunkManager.hasAllNeighbors(chunkKey)) {
                        TerrainGenerator::generateStructuresFor(chunkManager, chunkMap.at(chunkKey));
                    }
                }
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }
    }

//    auto end = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    std::cout << "Chunk Loading took: " << duration.count() << "milliseconds"
//              << std::endl;
}

void World::unloadChunks() {
    std::unordered_set<ChunkKey> chunksToUnload;
    int unloadDistanceChunks = m_renderDistance + 2;
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        ChunkKey chunkKey = it->first;
        const Ref<Chunk>& chunk = it->second;
        if (chunkKey.x < playerChunkKeyPos.x - unloadDistanceChunks ||
            chunkKey.x > playerChunkKeyPos.x + unloadDistanceChunks ||
            chunkKey.y < playerChunkKeyPos.y - unloadDistanceChunks ||
            chunkKey.y > playerChunkKeyPos.y + unloadDistanceChunks) {
            chunk->unload();
            it = chunkMap.erase(it);
        } else {
            ++it;
        }
    }
}


World::~World() {
    m_isRunning = false;
    for (auto &thread: m_chunkLoadThreads) {
        thread.join();
    }
}

void World::updateChunkMeshes() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    for (int i = 1; i <= m_renderDistance; i++) {
        int minX = playerChunkKeyPos.x - i;
        int maxX = playerChunkKeyPos.x + i;
        int minY = playerChunkKeyPos.y - i;
        int maxY = playerChunkKeyPos.y + i;
        for (int chunkX = minX; chunkX <= maxX; chunkX++) {
            for (int chunkY = minY; chunkY <= maxY; chunkY++) {
                std::unique_lock<std::mutex> lock(m_mainMutex);
                ChunkKey chunkKey = {chunkX, chunkY};
                if (!chunkManager.chunkExists(chunkKey)) continue;
                const Ref<Chunk>& chunk = chunkManager.getChunk(chunkKey);
                if (chunk->chunkState != ChunkState::FULLY_GENERATED) continue;
                if (chunk->chunkMeshState == ChunkMeshState::BUILT) continue;
                if (!chunkManager.hasAllNeighborsFullyGenerated(chunkKey)) continue;
                chunkManager.buildChunkMesh(chunkKey);
            }
        }
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
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
        Block block = chunkManager.getBlock(blockPos);
        if (block.id == Block::AIR) {
            lastAirBlockPos = blockPos;
            rayEnd += directionIncrement;
            continue;
        }
        prevLastRayCastBlockPos = lastRayCastBlockPos;
        lastRayCastBlockPos = blockPos;

        // calculate block break stage. 10 stages. 0 is no break, 10 is fully broken
        auto duration = std::chrono::steady_clock::now() - lastTime;
        auto durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(
                duration).count();
        int breakStage = static_cast<int>(durationMS / (MINING_DELAY_MS / 10));
        if (breakStage > 10) breakStage = 10;

        // breaking block
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            // check if player switched block aimed at. if so, reset time and break stage and return
            if (!compareVec3(lastRayCastBlockPos, prevLastRayCastBlockPos)) {
                player.blockBreakStage = 0;
                lastTime = std::chrono::steady_clock::now();
                return;
            }
            if (std::chrono::steady_clock::now() - lastTime <
                std::chrono::milliseconds(MINING_DELAY_MS)) {
                player.blockBreakStage = breakStage;
                return;
            }

            chunkManager.setBlock({blockPos.x, blockPos.y, blockPos.z}, Block(Block::AIR));
            player.blockBreakStage = 0;
            lastRayCastBlockPos = NULL_VECTOR;
            isFirstAction = false;
        }
            // placing block
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            if (!isFirstAction && std::chrono::steady_clock::now() - lastTime <
                                  std::chrono::milliseconds(PLACING_DELAY_MS)) {
                return;
            }
            chunkManager.setBlock(lastAirBlockPos, Block(player.inventory.getHeldItem()));
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
    lastRayCastBlockPos = NULL_VECTOR;
}

int World::getRenderDistance() const {
    return m_renderDistance;
}

void World::setRenderDistance(int renderDistance) {
    m_renderDistance = renderDistance;
}