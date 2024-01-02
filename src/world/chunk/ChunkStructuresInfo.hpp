//
// Created by Tony Adriansen on 1/1/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKSTRUCTURESINFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKSTRUCTURESINFO_HPP_

#include "ChunkInfo.hpp"
#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../generation/TerrainGenerator.hpp"

class Chunk;
class TerrainGenerator;

class ChunkStructuresInfo : public ChunkInfo {
 public:
  explicit ChunkStructuresInfo(Chunk *chunks[27], int seed);

  void generateStructureData(HeightMap &heightMap, TreeMap &treeMap);

 private:
  TerrainGenerator m_terrainGenerator;
  glm::ivec3 m_pos;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKSTRUCTURESINFO_HPP_
