//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP

#include "../../EngineConfig.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"
#include "../chunk/Chunk.hpp"

class TerrainGenerator {
 public:
  explicit TerrainGenerator(int seed);
  static void generateStructures(Chunk *chunk, HeightMap &heightMap, TreeMap &treeMap);

  void getHeightMap(const glm::ivec2 &startWorldPos, HeightMap &result) const;
  void getTreeMap(const glm::ivec2 &startWorldPos, TreeMap &result) const;
  static void generateTerrain(HeightMap &heightMap, Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK], int (&numBlocksPlaced)[CHUNKS_PER_STACK]);


  static void makeTree(const glm::ivec3 &pos, Chunk* chunk);

  static inline int WORLD_HEIGHT_XYZ(int x, int y, int z) {
    return x + (y << 5) + (z << 10);
  }

 private:
  int m_seed;
};

#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
