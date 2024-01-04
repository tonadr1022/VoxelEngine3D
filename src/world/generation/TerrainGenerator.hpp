//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP

#include "../../EngineConfig.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"

class TerrainGenerator {
 public:
  explicit TerrainGenerator(Chunk *(chunks)[27], int seed);
  void generateStructures(HeightMap &heightMap, TreeMap &treeMap);

  static void getHeightMap(const glm::ivec2 &startWorldPos, int seed, HeightMap &result);
  static void getTreeMap(const glm::ivec2 &startWorldPos, int seed, TreeMap &result);
  static void generateTerrain(HeightMap &heightMap, Block (&blocks)[CHUNK_VOLUME * CHUNKS_PER_STACK], int (&numBlocksPlaced)[CHUNKS_PER_STACK]);

  void makeTree(const glm::ivec3 &pos);

  static inline int WORLD_HEIGHT_XYZ(int x, int y, int z) {
    return x + (y << 5) + (z << 10);
  }

 private:
  void setBlock(int x, int y, int z, Block block);

  Chunk *m_chunks[27]{};

  int m_seed;
};

#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
