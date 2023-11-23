//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"
#include "chunk/ChunkKey.h"

void World::render() {
    chunkRenderer.startChunkRender();
    ChunkMap &chunkMap = chunkManager.getChunkMap();
    for (auto &chunk: chunkMap) {
        if (chunk.second.chunkMeshState == ChunkMeshState::BUILT) {
            chunkRenderer.render(chunk.second);
        }
    }
}

void World::update() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    loadChunks(playerChunkKeyPos);
    updateChunkMeshes(playerChunkKeyPos);
    unloadChunks();
}



void World::loadChunks(ChunkKey &playerChunkKeyPos, bool shouldLoadAll) {
    int loadDistanceChunks = 10;
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
//            }if (!chunkManager.chunkExists(chunkKey)) {
//                Chunk chunk(glm::vec2(chunkKey.x * CHUNK_WIDTH, chunkKey.y * CHUNK_WIDTH));
//                TerrainGenerator::generateTerrainFor(chunk);
//                chunk.chunkState = ChunkState::GENERATED;
//                chunkMap.emplace(chunkKey, chunk);
//                if (!shouldLoadAll && loadedChunks > 3) return;
//                loadedChunks++;
//            }
        }
    }
}

void World::updateChunkMeshes(ChunkKey &playerChunkKeyPos, bool shouldUpdateAll) {
    int renderDistanceChunks = 8;
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
                    // if any neighbor doesn't exist, don't build mesh
                    if (!chunkManager.chunkExists(leftNeighborChunkKey) ||
                        !chunkManager.chunkExists(rightNeighborChunkKey) ||
                        !chunkManager.chunkExists(frontNeighborChunkKey) ||
                        !chunkManager.chunkExists(backNeighborChunkKey)) {
                        continue;
                    }
                    Chunk leftNeighborChunk = chunkManager.getChunk(leftNeighborChunkKey);
                    Chunk rightNeighborChunk = chunkManager.getChunk(rightNeighborChunkKey);
                    Chunk frontNeighborChunk = chunkManager.getChunk(frontNeighborChunkKey);
                    Chunk backNeighborChunk = chunkManager.getChunk(backNeighborChunkKey);
                    chunk.buildMesh(leftNeighborChunk, rightNeighborChunk, frontNeighborChunk,
                                    backNeighborChunk);
                    ChunkRenderer::createGPUResources(chunk);
                    if (!shouldUpdateAll) return;

                }
            }
        }
    }
}


World::World(Player &player, Shader &chunkShader) : player(player), chunkRenderer(player.camera, chunkShader,
                                               ResourceManager::getTexture("texture_atlas")) {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();

    auto start = std::chrono::high_resolution_clock::now();

    loadChunks(playerChunkKeyPos, true);
    updateChunkMeshes(playerChunkKeyPos, true);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (duration.count() > 0.5)
        std::cout << "Chunk generation took: " << duration.count() << "milliseconds"
                  << std::endl;
}

void World::unloadChunks() {
    ChunkKey playerChunkKeyPos = player.getChunkKeyPos();
    int unloadDistanceChunks = 10;
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

