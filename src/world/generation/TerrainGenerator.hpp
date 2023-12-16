//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP



class Chunk;

class ChunkManager;

class TerrainGenerator {
public:
    TerrainGenerator();
    static void generateTerrainFor(Chunk &chunk);
    static void generateStructuresFor(ChunkManager& chunkManager, Chunk &chunk);
};


#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
