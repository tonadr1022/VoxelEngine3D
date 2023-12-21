//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP

#include "../../EngineConfig.hpp"

class Chunk;

class ChunkManager;

class TerrainGenerator {
public:
    TerrainGenerator();

    static void generateTerrainFor(const Ref<Chunk> &chunk);

    static void generateStructuresFor(ChunkManager &chunkManager, const Ref<Chunk> &chunk);

    static void makeTree(ChunkManager &chunkManager,const Ref<Chunk>& chunk, const glm::vec3 &position);
};


#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
