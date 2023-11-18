//
// Created by Tony Adriansen on 11/16/23.
//

#include "World.h"
#include "../resources/ResourceManager.h"


void World::render() {
//     iterate through the chunks and render them
    for (auto &chunk: chunkMap) {
        chunkRenderer.render(chunk.second);
    }
//    chunkRenderer.render(chunkMap.at({0, 0}));
}

void World::update() {
    player.update(window);
    auto playerPos = glm::vec3(0.0f);
    int playerChunkXPos = playerPos.x / CHUNK_WIDTH;
    int playerChunkYPos = playerPos.y / CHUNK_WIDTH;
    float renderDistance = player.camera.getFarPlane();
    int renderDistanceChunks = static_cast<int>(128);

    for (int chunkXPos = playerChunkXPos - renderDistanceChunks;
         chunkXPos < playerChunkXPos + renderDistanceChunks; chunkXPos+= CHUNK_WIDTH) {
        for (int chunkYPos = playerChunkYPos - renderDistanceChunks;
             chunkYPos < playerChunkYPos + renderDistanceChunks; chunkYPos+= CHUNK_WIDTH) {
            std::pair<int, int> chunkLocation = {chunkXPos, chunkYPos};
            if (chunkMap.find(chunkLocation) == chunkMap.end()) {
                Chunk chunk(glm::vec2(chunkXPos, chunkYPos));
                TerrainGenerator::generateTerrainFor(chunk);
                chunkMap.emplace(chunkLocation, chunk);
            }
        }
    }
//
//    std::pair<int, int> chunkLocation = {0, 0};
//    if (chunkMap.find(chunkLocation) == chunkMap.end()) {
//        Chunk chunk(glm::vec2(0, 0));
//        TerrainGenerator::generateTerrainFor(chunk);
//        chunkMap.emplace(chunkLocation, chunk);
//    }
}

World::World(GLFWwindow *window, Player &player, Shader &chunkShader) : window(
        window), player(player), chunkRenderer(player.camera, chunkShader,
                                               ResourceManager::getTexture("texture_atlas")) {
}
