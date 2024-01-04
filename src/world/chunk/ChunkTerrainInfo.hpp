//
// Created by Tony Adriansen on 1/1/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_

#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"
#include "../block/Block.hpp"
#include "ChunkInfo.hpp"


class ChunkTerrainInfo : public ChunkInfo {
 public:
  ChunkTerrainInfo(glm::ivec2 pos, int seed);

  void generateTerrainData();
  void applyTerrainDataToChunk(Chunk *(&chunk)[CHUNKS_PER_STACK]);

  HeightMap m_heightMap{};
  TreeMap m_treeMap{};
  int m_numBlocksPlaced[CHUNKS_PER_STACK]{};

 private:
  Block m_blocks[CHUNK_VOLUME * CHUNKS_PER_STACK]{};
  int m_seed;
  glm::ivec2 m_pos;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_
