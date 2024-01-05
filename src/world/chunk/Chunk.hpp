//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_HPP
#define VOXEL_ENGINE_CHUNK_HPP

#include "../../EngineConfig.hpp"
#include "../../AppConstants.hpp"

#include "ChunkMesh.hpp"
#include "ChunkMeshBuilder.hpp"


#include "../generation/TerrainGenerator.hpp"
#include "../../renderer/ViewFrustum.hpp"
#include "../../AppConstants.hpp"
#include "ChunkInfo.hpp"

enum class ChunkMeshState {
  BUILT = 0,
  UNBUILT,
};

enum class ChunkState {
  TERRAIN_GENERATED = 0,
  STRUCTURES_GENERATED,
  FULLY_GENERATED,
  UNGENERATED,
  CHANGED,
};

// Crashes when out of bounds
static inline int XYZ(int x, int y, int z) {
  return (z << 10 | y << 5 | x);
}

static inline int XYZ(glm::ivec3 pos) {
  return pos.z << 10 | pos.y << 5 | pos.x;
}

static inline int XY(int x, int y) {
  return x + (y << 5);
}

static inline int XY(glm::ivec2 &pos) {
  return pos.x + pos.y * CHUNK_SIZE;
}

static inline int MESH_XYZ(int x, int y, int z) {
  return (x + 1) + (y + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH
         + (z + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH;
}

static inline int LIGHT_XYZ(int x, int y, int z) {
  return (x + 14) + (y + 14) * CHUNK_LIGHT_INFO_WIDTH + (z + 14) * CHUNK_LIGHT_INFO_WIDTH * CHUNK_LIGHT_INFO_WIDTH;
}

class Chunk {
 public:
  Chunk() = delete;

  explicit Chunk(glm::ivec3 pos);

  ~Chunk();

  int m_numNonAirBlocks = 0;

  inline void setBlock(int x, int y, int z, Block block) {
    Block oldBlock = m_blocks[XYZ(x, y, z)];
    if (oldBlock != Block::AIR && block == Block::AIR) m_numNonAirBlocks--;
    if (oldBlock == Block::AIR && block != Block::AIR) m_numNonAirBlocks++;
    m_blocks[XYZ(x, y, z)] = block;
  }

  inline void setBlock(const glm::ivec3 &pos, Block block) {
    Block oldBlock = m_blocks[XYZ(pos)];
    if (oldBlock != Block::AIR && block == Block::AIR) m_numNonAirBlocks--;
    if (oldBlock == Block::AIR && block != Block::AIR) m_numNonAirBlocks++;
    m_blocks[XYZ(pos)] = block;
  }

//  inline void setBlock(int x, int y, int z, Block block) {
//    m_blocks[XYZ(x, y, z)] = block;
//  }

  void markDirty();

  static inline bool outOfBounds(int x, int y, int z) {
    return x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0
           || z >= CHUNK_SIZE;
  }

  [[nodiscard]] inline Block getBlock(int x, int y, int z) const {
    return m_blocks[XYZ(x, y, z)];
  }

  [[nodiscard]] inline Block getBlock(const glm::ivec3 &pos) const {
    return m_blocks[XYZ(pos)];
  }

  [[nodiscard]] inline Block getBlockFromIndex(int index) const {
    return m_blocks[index];
  }

  ChunkMeshState chunkMeshState;
  ChunkState chunkState;

  Block m_blocks[CHUNK_VOLUME]{};
  glm::ivec3 m_lightLevels[CHUNK_VOLUME]{};

  glm::ivec3 m_pos;
  glm::ivec3 m_worldPos;
  float m_firstBufferTime = 0;

  /**
   * @brief index 0 is opaque, 1 is transparent, 2 is other (floral etc)
   */
  ChunkMesh m_opaqueMesh;
  ChunkMesh m_transparentMesh;

  AABB m_boundingBox;
};



struct LightNode {
  glm::ivec3 pos;
  glm::ivec3 lightLevel;
};

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



class ChunkLightInfo : public ChunkInfo {
 public:

  explicit ChunkLightInfo(Chunk *chunks[27]);

  // TODO: do this in separate thread since we have the chunk pointer
  void applyLightDataToChunk(Chunk *chunk);

//  Block getBlockIncludingNeighborChunks(int x, int y, int z);

  void populateBlockArrayForLighting(Block (&blocks)[CHUNK_LIGHT_INFO_SIZE]);

  void generateLightingData();

 private:
  Chunk *m_chunks[27]{};
  glm::ivec3 m_lightLevelArray[CHUNK_LIGHT_INFO_SIZE]{};

  void propogateTorchLight(std::queue<LightNode> &torchlightQueue, Block (&blocks)[CHUNK_LIGHT_INFO_SIZE]);

  static inline void getPosFromLightArrIndex(glm::ivec3 &pos, int index) {
    int _i = index;
    pos.z = _i / (CHUNK_LIGHT_INFO_AREA);
    _i %= CHUNK_LIGHT_INFO_AREA;
    pos.y = _i / CHUNK_LIGHT_INFO_WIDTH - 14;
    pos.x = _i % CHUNK_LIGHT_INFO_WIDTH - 14;
  }


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


#endif //VOXEL_ENGINE_CHUNK_HPP
