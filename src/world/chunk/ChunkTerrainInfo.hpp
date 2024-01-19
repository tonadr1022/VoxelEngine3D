//
// Created by Tony Adriansen on 1/5/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_

#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"
#include "ChunkInfo.hpp"
#include "../block/Block.hpp"
#include "../generation/TerrainGenerator.hpp"

class Chunk;


class ChunkTerrainInfo : public ChunkInfo {
 public:
  ChunkTerrainInfo(glm::ivec2 pos, TerrainGenerator &terrainGenerator);

  void generateTerrainData();
  void applyTerrainDataToChunks(Chunk *(&chunk)[CHUNKS_PER_STACK]);

  HeightMap m_heightMap;
//  TemperatureMap m_temperatureMap;
//  PrecipitationMap m_precipitationMap;
  std::array<BiomeValue, CHUNK_AREA> m_biomeMap;
  int m_numBlocksPlaced[CHUNKS_PER_STACK]{};

 private:
  TerrainGenerator &m_terrainGenerator;
  Block m_blocks[CHUNK_VOLUME * CHUNKS_PER_STACK]{};
  glm::ivec2 m_pos;
};

#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKTERRAININFO_HPP_
