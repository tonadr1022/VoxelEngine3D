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

struct LightNode {
  glm::ivec3 pos;
  uint16_t lightLevel;

  LightNode(glm::ivec3 pPos, uint16_t pLightLevel)
      : pos(pPos), lightLevel(pLightLevel) {}
};

struct SunLightNode {
//  glm::ivec3 pos;
  uint8_t x;
  uint8_t y;
  uint16_t z;
  uint8_t lightLevel;

  SunLightNode(uint8_t px, uint8_t py, uint8_t pz, uint8_t pLightLevel)
      : x(px), y(py), z(pz), lightLevel(pLightLevel) {}
};

struct SunLightNodeWorld {
  glm::ivec3 pos;
  uint8_t lightLevel;

  SunLightNodeWorld(glm::ivec3 pPos, uint8_t pLightLevel)
      : pos(pPos), lightLevel(pLightLevel) {}
};

// Crashes when out of bounds
static inline int XYZ(int x, int y, int z) {
  return (z << 10 | y << 5 | x);
}

static inline glm::ivec3 XYZ_FROM_INDEX(int index) {
  return {index & 0x01F, (index >> 5) & 0x01F, (index >> 10) & 0x01F};
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

class Chunk {
 public:
  Chunk() = delete;

  explicit Chunk(glm::ivec3 pos);

  ~Chunk();

  int m_numNonAirBlocks = 0;

  // return true if x, y, or z are not between 0-31 inclusive.
  static inline bool isPosOutOfChunkBounds(int x, int y, int z) {
    return (x & 0b1111100000) || (y & 0b1111100000) || (z & 0b1111100000);
  }

// return true if x, y, or z are not between 0-31 inclusive.
  static inline bool isPosOutOfChunkBounds(const glm::ivec3 &pos) {
    return (pos.x & 0b1111100000) || (pos.y & 0b1111100000) || (pos.z & 0b1111100000);
  }

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

  [[nodiscard]] inline Block getBlock(int x, int y, int z) const {
    return m_blocks[XYZ(x, y, z)];
  }

  [[nodiscard]] inline Block getBlock(const glm::ivec3 &pos) const {
    return m_blocks[XYZ(pos)];
  }

  [[nodiscard]] inline glm::ivec3 getTorchLevel(const glm::ivec3 &pos) const {
//    return unpackLightLevel(m_lightLevels[XYZ(pos)]);
    if (!m_torchLightLevelsPtr) return {0, 0, 0};
    return Utils::unpackLightLevel(m_torchLightLevelsPtr.get()[XYZ(pos)]);
  }

  [[nodiscard]] inline uint16_t getTorchLevelPacked(const glm::ivec3 &pos) const {
    if (!m_torchLightLevelsPtr) return 0;
    return m_torchLightLevelsPtr.get()[XYZ(pos)];
  }

  inline void setTorchLevel(const glm::ivec3 &pos, const glm::ivec3 lightLevel) {
    if (!m_torchLightLevelsPtr) {
      allocateTorchLightLevels();
    }
    m_torchLightLevelsPtr.get()[XYZ(pos)] = Utils::packLightLevel(lightLevel);
//    m_lightLevels[XYZ(pos)] = Utils::packLightLevel(lightLevel);
  }

  inline void setTorchLevel(const glm::ivec3 &pos, uint16_t lightLevel) {
    if (!m_torchLightLevelsPtr) {
      allocateTorchLightLevels();
    }
    m_torchLightLevelsPtr.get()[XYZ(pos)] = lightLevel;
  }

  inline void setSunLightLevel(const glm::ivec3 &pos, uint8_t lightLevel) {
    if (!m_sunlightLevelsPtr) {
      allocateSunLightLevels();
    }
    m_sunlightLevelsPtr.get()[XYZ(pos)] = lightLevel;
  }

  void fillSunlightWithZ(int z, uint8_t lightLevel, bool AtOrAbove);

  [[nodiscard]] inline uint8_t getSunLightLevel(const glm::ivec3 &pos) const {
    if (!m_sunlightLevelsPtr) return 15;
    return m_sunlightLevelsPtr.get()[XYZ(pos)];
  }

  [[nodiscard]] inline Block getBlockFromIndex(int index) const {
    return m_blocks[index];
  }

  void setTorchLevelIncludingNeighborsOptimized(glm::ivec3 pos, uint16_t lightLevelPacked);

  void setBlockIncludingNeighborsOptimized(glm::ivec3 pos, Block block);

  void setSunlightIncludingNeighborsOptimized(glm::ivec3 pos, uint8_t lightLevel);

  [[nodiscard]] Block getBlockIncludingNeighborsOptimized(glm::ivec3 pos) const;

  [[nodiscard]] glm::ivec3 getTorchLevelIncludingNeighborsOptimized(glm::ivec3 pos) const;

  [[nodiscard]] uint16_t getTorchLevelPackedIncludingNeighborsOptimized(glm::ivec3 pos) const;

  [[nodiscard]] uint8_t getSunlightLevelIncludingNeighborsOptimized(glm::ivec3 pos) const;

  ChunkMeshState chunkMeshState;
  ChunkState chunkState;

  Block m_blocks[CHUNK_VOLUME]{};

  Chunk *m_neighborChunks[27]{};

  Scope<uint16_t[]> m_torchLightLevelsPtr = nullptr;
  Scope<uint8_t[]> m_sunlightLevelsPtr = nullptr;

  void allocateTorchLightLevels();
  void allocateSunLightLevels();

  glm::ivec3 m_pos;
  glm::ivec3 m_worldPos;
  float m_firstBufferTime = 0;

  ChunkMesh m_opaqueMesh;
  ChunkMesh m_transparentMesh;

  AABB m_boundingBox;

  // |  3   12  21
  // |    4   13  22
  // |      5   14  23
  // \-------------------y
  static constexpr const std::array<int, 9> HORIZONTAL_NEIGHBOR_INDICES = {3, 4, 5, 12, 13, 14, 21, 22, 23};

 private:
};

#endif //VOXEL_ENGINE_CHUNK_HPP
