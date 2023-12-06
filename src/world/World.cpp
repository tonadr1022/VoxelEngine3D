//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"
#include "chunk/ChunkKey.h"
#include "../utils/Utils.h"

World::World(GLFWwindow *window, Renderer &renderer) : window(window), renderer(renderer),
                                                       chunkRenderer(player.camera,
                                                                     ResourceManager::getTexture(
                                                                             "texture_atlas")) {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    auto start = std::chrono::high_resolution_clock::now();
    updateChunks();

    for (int i = 0; i < 2; i++) {
        m_chunkLoadThreads.emplace_back([&]() {
            while (m_isRunning) {
                updateChunks();
                // Optionally, add a sleep or yield to avoid busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (duration.count() > 0.5)
        std::cout << "Chunk generation took: " << duration.count() << "milliseconds"
                  << std::endl;
}

void World::render() {
//    std::unique_lock<std::mutex> lock(m_mainMutex);
    chunkRenderer.start();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto &chunk: chunkMap) {
        if (chunk.second.chunkMeshState == ChunkMeshState::BUILT) {
            chunkRenderer.render(chunk.second);
        }
    }
    // render block outline if aiming at a block
    if (static_cast<const glm::vec3>(lastRayCastBlockPos) != NULL_VECTOR) {
        // disable depth test so that the outline is always rendered on top
        renderer.renderBlockOutline(player.camera, lastRayCastBlockPos);
        renderer.renderBlockBreak(player.camera, lastRayCastBlockPos, player.blockBreakStage);
    }
}

void World::update() {
    castRay({player.camera.getPosition(), player.camera.getFront()});
    reloadChunksToReload();
    unloadChunks();
}

void World::updateChunks() {
    loadChunks();
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    updateChunkMeshes(playerChunkKeyPos);
//    unloadChunks();
}

void World::loadChunks() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int loadDistanceChunks = renderDistance + 2;
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (int chunkX = playerChunkKeyPos.x - loadDistanceChunks;
         chunkX < playerChunkKeyPos.x + loadDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - loadDistanceChunks;
             chunkY < playerChunkKeyPos.y + loadDistanceChunks; chunkY++) {
            ChunkKey chunkKey = {chunkX, chunkY};
            std::unique_lock<std::mutex> lock(m_mainMutex);
            if (chunkMap.find(chunkKey) == chunkMap.end()) {
                Chunk chunk(glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                TerrainGenerator::generateTerrainFor(chunk);
                chunk.chunkState = ChunkState::GENERATED;
                chunkMap.emplace(chunkKey, chunk);
            }
        }
    }
}



void World::unloadChunks() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int unloadDistanceChunks = renderDistance + 2;
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        std::unique_lock<std::mutex> lock(m_mainMutex);
        ChunkKey chunkKey = it->first;
        Chunk &chunk = it->second;
        if (chunkKey.x < playerChunkKeyPos.x - unloadDistanceChunks ||
            chunkKey.x > playerChunkKeyPos.x + unloadDistanceChunks ||
            chunkKey.y < playerChunkKeyPos.y - unloadDistanceChunks ||
            chunkKey.y > playerChunkKeyPos.y + unloadDistanceChunks) {
            chunk.unload();
            it = chunkMap.erase(it);
        } else {
            ++it;
        }
    }
}

void World::addEvent(std::unique_ptr<IEvent> event) {
    events.push_back(std::move(event));
}

World::~World() {
    m_isRunning = false;
    for (auto &thread: m_chunkLoadThreads) {
        thread.join();
    }
}

void World::reloadChunksToReload() {
    for (auto &chunkKey: chunksToReload) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//        std::unique_lock<std::mutex> lock(m_mainMutex);
        chunkManager.updateChunkMesh(chunkKey);
    }
    chunksToReload.clear();
}

void World::updateChunkMeshes(ChunkKey &playerChunkKeyPos) {
    for (int chunkX = playerChunkKeyPos.x - (renderDistance);
         chunkX < playerChunkKeyPos.x + (renderDistance); chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - renderDistance;
             chunkY < playerChunkKeyPos.y + renderDistance; chunkY++) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            std::unique_lock<std::mutex> lock(m_mainMutex);
            ChunkKey chunkKey = {chunkX, chunkY};
            Chunk& chunk = chunkManager.getChunk(chunkKey);
            if (chunk.chunkMeshState == ChunkMeshState::BUILT) continue;
            chunkManager.updateChunkMesh(chunkKey);
        }
    }
}

void World::handleChunkUpdates(Chunk& chunk, ChunkKey chunkKey, int chunkX, int chunkY) {
    chunksToReload.push_back(chunkKey);
    chunk.markDirty();
    // need to update adjacent chunks if block is on border
    if (chunkX == 0) {
        ChunkKey backChunkKey = ChunkManager::calculateNeighborChunkKey(HorizontalDirection::BACK, chunkKey);
        Chunk &backChunk = chunkManager.getChunk(backChunkKey);
        chunksToReload.push_back(backChunkKey);
        backChunk.markDirty();
    } else if (chunkX == CHUNK_WIDTH - 1) {
        ChunkKey frontChunkKey = ChunkManager::calculateNeighborChunkKey(HorizontalDirection::FRONT, chunkKey);
        Chunk &frontChunk = chunkManager.getChunk(frontChunkKey);
        chunksToReload.push_back(frontChunkKey);
        frontChunk.markDirty();
    }
    if (chunkY == 0) {
        ChunkKey leftChunkKey = ChunkManager::calculateNeighborChunkKey(HorizontalDirection::LEFT, chunkKey);
        Chunk &leftChunk = chunkManager.getChunk(leftChunkKey);
        chunksToReload.push_back(leftChunkKey);
        leftChunk.markDirty();
    } else if (chunkY == CHUNK_WIDTH - 1) {
        ChunkKey rightChunkKey = ChunkManager::calculateNeighborChunkKey(HorizontalDirection::RIGHT, chunkKey);
        Chunk &rightChunk = chunkManager.getChunk(rightChunkKey);
        chunksToReload.push_back(rightChunkKey);
        rightChunk.markDirty();
    }
}

void World::setBlock(glm::ivec3 position, Block block) {
    auto chunkKey = ChunkManager::getChunkKeyByWorldLocation(position.x, position.y);
    Chunk &chunk = chunkManager.getChunk(chunkKey);
    int chunkX = Utils::positiveModulo(position.x, CHUNK_WIDTH);
    int chunkY = Utils::positiveModulo(position.y, CHUNK_WIDTH);

    chunk.setBlock(chunkX, chunkY, position.z, block);
    handleChunkUpdates(chunk, chunkKey, chunkX, chunkY);
}

void World::castRay(Ray ray) {
    glm::ivec3 lastAirBlockPos = NULL_VECTOR;
    glm::vec3 rayStart = ray.origin;
    glm::vec3 rayEnd = ray.origin;
    glm::vec3 directionIncrement = glm::normalize(ray.direction) * 0.05f;
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    static bool isFirstAction = true;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE &&
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        player.blockBreakStage = 0;
        isFirstAction = true;
    }
    while (glm::distance(rayStart, rayEnd) < 10.0f) {
        glm::ivec3 blockPos = {floor(rayEnd.x), floor(rayEnd.y), floor(rayEnd.z)};
        lastRayCastBlockPos = blockPos;
        Block block = chunkManager.getBlock(blockPos);
        if (block.id != Block::AIR) {
            // calculate block break stage. 10 stages. 0 is no break, 10 is fully broken
            auto duration = std::chrono::steady_clock::now() - lastTime;
            long durationMS = std::chrono::duration_cast<std::chrono::milliseconds>(
                    duration).count();
            int breakStage = durationMS / (MINING_DELAY_MS / 10);
            if (breakStage > 10) breakStage = 10;

            // breaking block
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                if (std::chrono::steady_clock::now() - lastTime <
                    std::chrono::milliseconds(MINING_DELAY_MS)) {
                    player.blockBreakStage = breakStage;
                    return;
                }

                setBlock({blockPos.x, blockPos.y, blockPos.z}, Block(Block::AIR));
                isFirstAction = false;
            }
                // placing block
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                if (!isFirstAction && std::chrono::steady_clock::now() - lastTime <
                                      std::chrono::milliseconds(PLACING_DELAY_MS)) {
                    return;
                }
                setBlock(lastAirBlockPos, Block(player.inventory.getHeldItem()));

                isFirstAction = false;
            }
            lastTime = std::chrono::steady_clock::now();
            return;
        } else {
            lastAirBlockPos = blockPos;
        }
        rayEnd += directionIncrement;
    }
    player.blockBreakStage = 0;
    lastRayCastBlockPos = NULL_VECTOR;
}