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


  static constexpr uint32_t RED_MASK = 0xF00;
  static constexpr uint32_t GREEN_MASK = 0x0F0;
  static constexpr uint32_t BLUE_MASK = 0x00F;

  static inline glm::ivec3 unpackLightLevel(uint32_t level) {
    return {
        static_cast<int8_t>((level & RED_MASK) >> 8),
        static_cast<int8_t>((level & GREEN_MASK) >> 4),
        static_cast<int8_t>((level & BLUE_MASK)),
    };
  }
};


#endif //VOXEL_ENGINE_SRC_WORLD_CHUNK_CHUNKLIGHTINFO_HPP_
