//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_WORLD_H
#define VOXEL_ENGINE_WORLD_H

#include "../Config.h"
#include "../player/Player.h"
#include "../camera/Camera.h"
#include "generation/TerrainGenerator.h"
#include "chunk/ChunkRenderer.h"
#include "chunk/ChunkManager.h"
#include <set>
#include <future>

class World {
public:
    World(Player &player, Shader &chunkShader);

    void update();


    void render();


private:
    int chunksToLoadPerFrame = 1;
    int chunksLoadedThisFrame = 0;

    void loadChunks(ChunkKey &playerChunkKeyPos, bool shouldLoadAll = false);
    void updateChunkMeshes(ChunkKey &playerChunkKeyPos, bool shouldUpdateAll = false);

    void unloadChunks();
    Player &player;
    ChunkRenderer chunkRenderer;
    ChunkManager chunkManager;
};


#endif //VOXEL_ENGINE_WORLD_H
