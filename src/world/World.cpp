//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"


void World::render() {
//     iterate through the chunks and render them
//    auto playerPos = glm::vec3(0.0f);
    auto playerPos = player.getPosition();
    int playerChunkXPos = playerPos.x / CHUNK_WIDTH;
    int playerChunkYPos = playerPos.y / CHUNK_WIDTH;
    int renderDistanceChunks = 12;

    for (int chunkX = playerChunkXPos - renderDistanceChunks;
         chunkX < playerChunkXPos + renderDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkYPos - renderDistanceChunks;
             chunkY < playerChunkYPos + renderDistanceChunks; chunkY++) {
            std::pair<int, int> chunkKey = {chunkX, chunkY};
            if (chunkMap.find(chunkKey) != chunkMap.end()) {
                chunkRenderer.render(chunkMap.at(chunkKey));
            }
        }
    }


//    for (auto &chunk: chunkMap) {
//        chunkRenderer.render(chunk.second);
//    }
//    chunkRenderer.render(chunkMap.at({0, 0}));
}

void World::update() {
    player.update(window);
    auto playerPos = player.getPosition();
    int playerChunkXPos = playerPos.x / CHUNK_WIDTH;
    int playerChunkYPos = playerPos.y / CHUNK_WIDTH;
    int loadDistanceChunks = static_cast<int>(14);

    for (int chunkX = playerChunkXPos - loadDistanceChunks;
         chunkX < playerChunkXPos + loadDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkYPos - loadDistanceChunks;
             chunkY < playerChunkYPos + loadDistanceChunks; chunkY ++) {
            std::pair<int, int> chunkKey = {chunkX, chunkY};
            if (chunkMap.find(chunkKey) == chunkMap.end()) {
                Chunk chunk(glm::vec2(chunkX * CHUNK_WIDTH, chunkY * CHUNK_WIDTH), *this);
                TerrainGenerator::generateTerrainFor(chunk);
                chunkMap.emplace(chunkKey, chunk);
            }
        }
    }
}

World::World(GLFWwindow *window, Player &player, Shader &chunkShader) : window(
        window), player(player), chunkRenderer(player.camera, chunkShader,
                                               ResourceManager::getTexture("texture_atlas")) {
}

Chunk &World::getChunk(int x, int y) {
    std::pair<int, int> chunkKey = {x / CHUNK_WIDTH, y / CHUNK_WIDTH};
    auto it = chunkMap.find(chunkKey);
    if (it == chunkMap.end()) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(x / CHUNK_WIDTH) + ", " + std::to_string(y / CHUNK_WIDTH) + "\n");
    }
    return it->second;
}
