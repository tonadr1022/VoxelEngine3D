//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"
#include "chunk/ChunkKey.h"

void World::render() {
    // render chunks
    chunkRenderer.start();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto &chunk: chunkMap) {
        if (chunk.second.chunkMeshState == ChunkMeshState::BUILT) {
            chunkRenderer.render(chunk.second);
        }
    }

    // render block outline if aiming at a block
    if (static_cast<const glm::vec3>(lastRayCastBlockPos) != NULL_VECTOR) {
        renderer.renderBlockOutline(lastRayCastBlockPos);
    }
}

void World::update() {
    castRay({player.camera.getPosition(), player.camera.getFront()});
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    loadChunks(playerChunkKeyPos);
    chunkManager.updateChunkMeshes(playerChunkKeyPos, renderDistance);
    unloadChunks();
}


void World::loadChunks(ChunkKey &playerChunkKeyPos, bool shouldLoadAll) {
    int loadDistanceChunks = renderDistance + 2;
    int loadedChunks = 0;
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (int chunkX = playerChunkKeyPos.x - loadDistanceChunks;
         chunkX < playerChunkKeyPos.x + loadDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - loadDistanceChunks;
             chunkY < playerChunkKeyPos.y + loadDistanceChunks; chunkY++) {
            ChunkKey chunkKey = {chunkX, chunkY};
            if (chunkMap.find(chunkKey) == chunkMap.end()) {
                Chunk chunk(glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                TerrainGenerator::generateTerrainFor(chunk);
                chunk.chunkState = ChunkState::GENERATED;
                chunkMap.emplace(chunkKey, chunk);
                if (!shouldLoadAll) return;
                loadedChunks++;
            }
        }
    }
}

World::World(GLFWwindow *window, Player &player, Renderer &renderer) : window(window),
                                                   player(player),
                                                   renderer(renderer),
                                                   chunkRenderer(player.camera,
                                                                 ResourceManager::getTexture(
                                                                         "texture_atlas")) {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();

    auto start = std::chrono::high_resolution_clock::now();

    loadChunks(playerChunkKeyPos, true);
    chunkManager.updateChunkMeshes(playerChunkKeyPos, renderDistance, true);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (duration.count() > 0.5)
        std::cout << "Chunk generation took: " << duration.count() << "milliseconds"
                  << std::endl;
}

void World::unloadChunks() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int unloadDistanceChunks = renderDistance + 2;
    ChunkMap &chunkMap = chunkManager.getChunkMap();

    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
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

void World::castRay(Ray ray) {
    glm::ivec3 lastAirBlockPos = NULL_VECTOR;
    glm::vec3 rayStart = ray.origin;
    glm::vec3 rayEnd = ray.origin;
    glm::vec3 direction = glm::normalize(ray.direction) * 0.05f;
    static std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    static bool isFirstAction = true;
    int steps = 0;
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE &&
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        isFirstAction = true;
    }
    while (glm::distance(rayStart, rayEnd) < 10.0f) {
        glm::ivec3 blockPos = {floor(rayEnd.x), floor(rayEnd.y), floor(rayEnd.z)};
        lastRayCastBlockPos = blockPos;
        Block block = chunkManager.getBlock(blockPos);
        if (block.id != Block::AIR) {
            // breaking block
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                if (!isFirstAction && std::chrono::steady_clock::now() - lastTime < std::chrono::milliseconds(MINING_DELAY_MS)) {
                    return;
                }
                chunkManager.setBlockAndHandleChunkUpdates({blockPos.x, blockPos.y, blockPos.z}, Block(Block::AIR));
                isFirstAction = false;
            }
            // placing block
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
                if (!isFirstAction && std::chrono::steady_clock::now() - lastTime < std::chrono::milliseconds(PLACING_DELAY_MS)) {
                    return;
                }
                std::cout << "last air block pos: " << lastAirBlockPos.x << ", " << lastAirBlockPos.y << ", " << lastAirBlockPos.z << std::endl;
                chunkManager.setBlockAndHandleChunkUpdates(lastAirBlockPos, Block(Block::STONE));
                isFirstAction = false;
            }
            lastTime = std::chrono::steady_clock::now();
            return;
        } else {
            lastAirBlockPos = blockPos;
        }
        rayEnd += direction;
        steps++;
    }
    lastRayCastBlockPos = NULL_VECTOR;
}

void World::addEvent(std::unique_ptr<IEvent> event) {
    events.push_back(std::move(event));
}

