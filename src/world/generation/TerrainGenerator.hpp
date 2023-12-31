//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_TERRAINGENERATOR_HPP
#define VOXEL_ENGINE_TERRAINGENERATOR_HPP

#include "../../EngineConfig.hpp"
#include "../block/Block.hpp"
#include "../../AppConstants.hpp"

class Chunk;

class ChunkManager;

class TerrainGenerator {
 public:
  explicit TerrainGenerator(Chunk *(chunks)[9], int seed);
  void generateStructures();

  static void getHeightMap(glm::ivec2 startWorldPos, int seed, std::array<int, CHUNK_AREA> &result);
  static void generateTerrain(const glm::ivec2 &worldPos, int seed, Block (&blocks)[CHUNK_VOLUME]);

  void makeTree(const glm::ivec3 &pos);

 private:
  void setBlock(int x, int y, int z, Block block);

  Chunk *m_chunks[9]{};

  int m_seed;
};

#endif //VOXEL_ENGINE_TERRAINGENERATOR_HPP
