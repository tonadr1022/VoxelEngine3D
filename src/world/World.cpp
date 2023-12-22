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


World::World(Renderer &renderer, int seed) : renderer(renderer),
                                             chunkRenderer(player.camera),
                                             m_terrainGenerator(seed) {
    BlockDB::loadData("../resources/blocks/");

    {
        Timer timer("World initialization");
        updateChunks();
    }
//    glm::ivec2 pos = {0,0};
//    m_load_info_map.emplace(pos, std::make_unique<ChunkLoadInfo>(pos, 1));
//    m_load_info_map.at(pos)->process();


//    for (int i = 0; i < 1; i++) {
//        m_chunkLoadThreads.emplace_back([&]() {
//            while (m_isRunning) {
//                updateChunks();
//                std::this_thread::sleep_for(std::chrono::milliseconds(10));
//            }
//        });
//    }
}

World::~World() {
    m_isRunning = false;
    for (auto &thread: m_chunkLoadThreads) {
        thread.join();
    }
}

void sortChunksByDistance(std::vector<Ref<Chunk>> &chunks, const glm::vec3 &playerPos) {
    std::sort(chunks.begin(), chunks.end(), [&playerPos](const Ref<Chunk> &a, const Ref<Chunk> &b) {
        return (glm::distance(a.get()->location(), playerPos) >
                glm::distance(b.get()->location(), playerPos));
    });
}

void World::render() {
    chunkRenderer.start();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    std::vector<Ref<Chunk>> chunksToRender;
    for (auto &chunkEntry: chunkMap) {
        if (chunkEntry.second->chunkMeshState == ChunkMeshState::BUILT) {
            chunksToRender.push_back(chunkEntry.second);
        }
    }

    sortChunksByDistance(chunksToRender, player.getPosition());

    for (auto &chunk: chunksToRender) {
        chunkRenderer.render(chunk);
    }

    // render block break and outline if a block is being aimed at
    if (static_cast<const glm::vec3>(lastRayCastBlockPos) != NULL_VECTOR) {
        renderer.renderBlockOutline(player.camera, lastRayCastBlockPos);
        renderer.renderBlockBreak(player.camera, lastRayCastBlockPos, player.blockBreakStage);
    }
    renderer.renderCrossHair();
    renderDebugGui();
}

void World::update() {
    unloadChunks();
    updateChunks();
    castPlayerAimRay({player.camera.getPosition(), player.camera.getFront()});
    chunkManager.remeshChunksToRemesh();
}


void World::updateChunks() {
    loadChunks();
    updateChunkMeshes();
}

void World::loadChunks() {
    int firstPassLoadDistance = m_renderDistance + 2;
    int secondPassLoadDistance = firstPassLoadDistance - 1;

    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    ChunkMap &chunkMap = chunkManager.getChunkMap();

    bool foundChunk = false;
    for (int i = 1; i <= firstPassLoadDistance && !foundChunk; i++) {
        int minX = playerChunkKeyPos.x - i;
        int maxX = playerChunkKeyPos.x + i;
        int minY = playerChunkKeyPos.y - i;
        int maxY = playerChunkKeyPos.y + i;
        for (int chunkX = minX; chunkX <= maxX && !foundChunk; chunkX++) {
            for (int chunkY = minY; chunkY <= maxY && !foundChunk; chunkY++) {
                ChunkKey chunkKey = {chunkX, chunkY};
                auto it = chunkMap.find(chunkKey);
                if (it == chunkMap.end()) {
                    const Ref<Chunk> chunk = std::make_shared<Chunk>(
                            glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                    m_terrainGenerator.generateTerrainFor(chunk);
                    chunkMap.emplace(chunkKey, chunk);
                    foundChunk = true;
                }
            }
        }
    }

    foundChunk = false;
    for (int i = 1; i <= secondPassLoadDistance && !foundChunk; i++) {
        int minX = playerChunkKeyPos.x - i;
        int maxX = playerChunkKeyPos.x + i;
        int minY = playerChunkKeyPos.y - i;
        int maxY = playerChunkKeyPos.y + i;

        for (int chunkX = minX; chunkX <= maxX && !foundChunk; chunkX++) {
            for (int chunkY = minY; chunkY <= maxY && !foundChunk; chunkY++) {
                ChunkKey chunkKey = {chunkX, chunkY};
                auto it = chunkMap.find(chunkKey);
                if (it != chunkMap.end() &&
                    it->second->chunkState == ChunkState::TERRAIN_GENERATED &&
                    chunkManager.hasAllNeighbors(chunkKey)) {
                    m_terrainGenerator.generateStructuresFor(chunkManager, it->second);
                    foundChunk = true;
                }
            }
        }
    }
}

void World::unloadChunks() {
    std::unordered_set<ChunkKey> chunksToUnload;
    int unloadDistanceChunks = m_renderDistance + 32;
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        ChunkKey chunkKey = it->first;
        const Ref<Chunk> &chunk = it->second;
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

void World::updateChunkMeshes() {
    Timer t("mesh");
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    for (int i = 1; i <= m_renderDistance; i++) {
        int minX = playerChunkKeyPos.x - i;
        int maxX = playerChunkKeyPos.x + i;
        int minY = playerChunkKeyPos.y - i;
        int maxY = playerChunkKeyPos.y + i;
        for (int chunkX = minX; chunkX <= maxX; chunkX++) {
            for (int chunkY = minY; chunkY <= maxY; chunkY++) {
                ChunkKey chunkKey = {chunkX, chunkY};
                if (!chunkManager.chunkExists(chunkKey)) continue;
                const Ref<Chunk> &chunk = chunkManager.getChunk(chunkKey);
                if (chunk->chunkState != ChunkState::FULLY_GENERATED) continue;
                if (chunk->chunkMeshState == ChunkMeshState::BUILT) continue;
                if (!chunkManager.hasAllNeighborsFullyGenerated(chunkKey)) continue;
                chunkManager.buildChunkMesh(chunkKey);
                return;
            }
        }
    }
    std::cout << "didnt build" << std::endl;
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
            Block block = chunkManager.getBlock(blockPos);
            if (block == Block::AIR) {
                lastAirBlockPos = blockPos;
                rayEnd += directionIncrement;
                continue;
            }
        } catch (std::exception &e) {
            return;
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
        if (Mouse::isPressed(GLFW_MOUSE_BUTTON_LEFT)) {
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

            chunkManager.setBlock({blockPos.x, blockPos.y, blockPos.z}, Block::AIR);
            player.blockBreakStage = 0;
            lastRayCastBlockPos = NULL_VECTOR;
            isFirstAction = false;
        }
            // placing block
        else if (Mouse::isPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
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

void World::initialize(const std::function<void()> &callback) {

    m_isInitializing = true;
    std::future initializationFuture = std::async(std::launch::async, [this, callback] {
        Timer timer("World initialization");
        updateChunks();
    });

    while (initializationFuture.wait_for(std::chrono::milliseconds(10)) !=
           std::future_status::ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "Loading" << std::endl;
    }

    m_isInitializing = false;
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

    player.renderDebugGui();

    ImGui::End();
}

