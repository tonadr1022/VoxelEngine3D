//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"
#include "chunk/ChunkKey.h"
#include <thread>


void World::render() {
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto &chunk: chunkMap) {
        if (chunk.second.chunkMeshState == ChunkMeshState::BUILT) {
            chunkRenderer.render(chunk.second);
        }
    }
}

void World::update() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int loadDistanceChunks = 16;
    int renderDistanceChunks = 14;
    ChunkMap &chunkMap = chunkManager.getChunkMap();

    for (int chunkX = playerChunkKeyPos.x - loadDistanceChunks;
         chunkX < playerChunkKeyPos.x + loadDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - loadDistanceChunks;
             chunkY < playerChunkKeyPos.y + loadDistanceChunks; chunkY++) {
            ChunkKey chunkKey = {chunkX, chunkY};
            if (!chunkManager.chunkExists(chunkKey)) {
                std::cout << "Generating chunk at: " << chunkKey.x << ", " << chunkKey.y
                          << std::endl;
                Chunk chunk(glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
                TerrainGenerator::generateTerrainFor(chunk);
                chunk.chunkState = ChunkState::GENERATED;
                chunkMap.emplace(chunkKey, chunk);
            }
        }
    }
    unloadChunks();

    for (int chunkX = playerChunkKeyPos.x - renderDistanceChunks;
         chunkX < playerChunkKeyPos.x + renderDistanceChunks; chunkX++) {
        for (int chunkY = playerChunkKeyPos.y - renderDistanceChunks;
             chunkY < playerChunkKeyPos.y + renderDistanceChunks; chunkY++) {
            ChunkKey chunkKey = {chunkX, chunkY};
            if (chunkManager.chunkExists(chunkKey)) {
                Chunk &chunk = chunkManager.getChunk(chunkKey);
                if (chunk.chunkState == ChunkState::GENERATED &&
                    chunk.chunkMeshState == ChunkMeshState::UNBUILT) {
                    ChunkKey leftNeighborChunkKey = {chunkKey.x, chunkKey.y - 1};
                    ChunkKey rightNeighborChunkKey = {chunkKey.x, chunkKey.y + 1};
                    ChunkKey frontNeighborChunkKey = {chunkKey.x + 1, chunkKey.y};
                    ChunkKey backNeighborChunkKey = {chunkKey.x - 1, chunkKey.y};
                    Chunk leftNeighborChunk = chunkManager.getChunk(leftNeighborChunkKey);
                    Chunk rightNeighborChunk = chunkManager.getChunk(rightNeighborChunkKey);
                    Chunk frontNeighborChunk = chunkManager.getChunk(frontNeighborChunkKey);
                    Chunk backNeighborChunk = chunkManager.getChunk(backNeighborChunkKey);
                    chunk.buildMesh(leftNeighborChunk, rightNeighborChunk, frontNeighborChunk,
                                    backNeighborChunk);
                    ChunkRenderer::createGPUResources(chunk);
                }
            }
        }
    }
}

World::World(GLFWwindow *window, Player &player, Shader &chunkShader) : window(
        window), player(player), chunkRenderer(player.camera, chunkShader,
                                               ResourceManager::getTexture("texture_atlas")) {

}


void World::unloadChunks() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int unloadDistanceChunks = 16;
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

