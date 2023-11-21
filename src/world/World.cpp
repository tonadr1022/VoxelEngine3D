//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"


void World::render() {

    auto playerChunkKeyPos = player.getChunkKeyPos();
    int renderDistanceChunks = 14;
    for (int chunkX = playerChunkKeyPos.first - renderDistanceChunks;
         chunkX < playerChunkKeyPos.first + renderDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.second - renderDistanceChunks;
             chunkY < playerChunkKeyPos.second + renderDistanceChunks; chunkY++) {
            std::pair<int, int> chunkKey = {chunkX, chunkY};
            if (chunkMap.find(chunkKey) != chunkMap.end()) {
                chunkRenderer.render(chunkMap.at(chunkKey));
            }
        }
    }
}

void World::update() {
    std::pair<int, int> playerChunkKeyPos = player.getChunkKeyPos();
    int loadDistanceChunks = static_cast<int>(16);
    for (int chunkX = playerChunkKeyPos.first - loadDistanceChunks;
         chunkX < playerChunkKeyPos.first + loadDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.second - loadDistanceChunks;
             chunkY < playerChunkKeyPos.second + loadDistanceChunks; chunkY++) {
            std::pair<int, int> chunkKey = {chunkX, chunkY};
            if (chunkMap.find(chunkKey) == chunkMap.end()) {
                Chunk chunk(glm::vec2(chunkX * CHUNK_WIDTH, chunkY * CHUNK_WIDTH), *this);
                TerrainGenerator::generateTerrainFor(chunk);
                chunkMap.emplace(chunkKey, chunk);
            }
        }
    }
    unloadChunks();
}

World::World(GLFWwindow *window, Player &player, Shader &chunkShader) : window(
        window), player(player), chunkRenderer(player.camera, chunkShader,
                                               ResourceManager::getTexture("texture_atlas"))  {


}

Chunk &World::getChunk(int x, int y) {
    std::pair<int, int> chunkKey = {x / CHUNK_WIDTH, y / CHUNK_WIDTH};
    auto it = chunkMap.find(chunkKey);
    if (it == chunkMap.end()) {
        throw std::runtime_error(
                "Chunk not found at: " + std::to_string(x / CHUNK_WIDTH) + ", " +
                std::to_string(y / CHUNK_WIDTH) + "\n");
    }
    return it->second;
}

void World::unloadChunks() {
    std::pair<int, int> playerChunkKeyPos = player.getChunkKeyPos();
    int unloadDistanceChunks = 16;
    for (auto it = chunkMap.begin(); it != chunkMap.end();) {
        std::pair<int, int> chunkKey = it->first;
        Chunk &chunk = it->second;
        if (chunkKey.first < playerChunkKeyPos.first - unloadDistanceChunks ||
            chunkKey.first > playerChunkKeyPos.first + unloadDistanceChunks ||
            chunkKey.second < playerChunkKeyPos.second - unloadDistanceChunks ||
            chunkKey.second > playerChunkKeyPos.second + unloadDistanceChunks) {
            chunk.unload();
            it = chunkMap.erase(it);
        } else {
            ++it;
        }
    }
}

