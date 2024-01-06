//
// Created by Tony Adriansen on 1/5/2024.
//

#ifndef VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKLIGHTINFO_HPP_
#define VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKLIGHTINFO_HPP_

#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"
#include "ChunkInfo.hpp"
#include "../block/Block.hpp"


struct LightNode {
  glm::ivec3 pos;
  glm::ivec3 lightLevel;
};

class Chunk;

class ChunkLightInfo : public ChunkInfo {
 public:

  explicit ChunkLightInfo(Chunk *chunks[27]);

//  Block getBlockIncludingNeighborChunks(int x, int y, int z);

//  void populateBlockArrayForLighting(Block (&blocks)[CHUNK_LIGHT_INFO_SIZE]);

  void generateLightingData();



 private:
  Block getBlock(const glm::ivec3 &pos);
  Chunk *m_chunks[27]{};

  void propagateTorchLight(std::queue<LightNode> &torchlightQueue);
};

//class TorchLightInfo {
// public:
//
// private:
//  std::uni
//};

//class TorchLightInfoLayer {
// public:
//  void setTorchLight(uint16_t lightLevel, int localX, int localY);
//  uint16_t getTorchLight(int localX, int localY);
//
// private:
//  uint16_t m_lightLevelLayer[CHUNK_AREA];
//};


#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKLIGHTINFO_HPP_
