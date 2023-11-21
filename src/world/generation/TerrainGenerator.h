//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_H
#define VOXEL_ENGINE_TERRAINGENERATOR_H


class Chunk;

class TerrainGenerator {
public:
    TerrainGenerator();
    static void generateTerrainFor(Chunk &chunk);

private:
    static void setBuriedBlockStates(Chunk &chunk);

};


#endif //VOXEL_ENGINE_TERRAINGENERATOR_H
