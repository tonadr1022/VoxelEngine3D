//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_HPP
#define VOXEL_ENGINE_CHUNK_HPP

#include "ChunkMesh.hpp"
#include "ChunkMeshBuilder.hpp"
#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"
#include "../generation/TerrainGenerator.hpp"
#include "../../renderer/ViewFrustum.hpp"

enum class ChunkMeshState {
  BUILT = 0,
  UNBUILT,
};

enum class ChunkState {
  TERRAIN_GENERATED = 0,
  FULLY_GENERATED,
  UNGENERATED,
  CHANGED,
  UNDEFINED,
};

static inline int XYZ(int x, int y, int z) {
  return x + (y * CHUNK_WIDTH) + (z * CHUNK_AREA);
//  return z * CHUNK_AREA + y * CHUNK_WIDTH + x;
}

static inline int XYZ(glm::ivec3 pos) {
  return pos.z * CHUNK_AREA + pos.y * CHUNK_WIDTH + pos.x;
//  return pos.x + (pos.y * CHUNK_WIDTH) + (pos.z * CHUNK_AREA);
}

static inline int XY(int x, int y) {
  return x + y * CHUNK_WIDTH;
}

static inline int XY(glm::ivec2 &pos) {
  return pos.x + pos.y * CHUNK_WIDTH;
}

static inline int MESH_XYZ(int x, int y, int z) {
  return (x + 1) + (y + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH
      + (z + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH;
}

class Chunk {
 public:
  Chunk() = delete;
  explicit Chunk(glm::ivec2 location);
  ~Chunk();

  inline void setBlock(int x, int y, int z, Block block) {
    m_blocks[XYZ(x, y, z)] = block;
  }

  void markDirty();

  static inline bool outOfBounds(int x, int y, int z) {
    return x < 0 || x >= CHUNK_WIDTH || y < 0 || y >= CHUNK_WIDTH || z < 0
        || z >= CHUNK_HEIGHT;
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
  glm::ivec2 m_pos;
  glm::ivec2 m_worldPos;
  float m_firstBufferTime = 0;

  /**
   * @brief index 0 is opaque, 1 is transparent, 2 is other (floral etc)
   */
  ChunkMesh m_opaqueMesh;
  ChunkMesh m_transparentMesh;

  AABB m_boundingBox;
};

class ChunkInfo {
 public:
  ChunkInfo() : m_done(false) {
  }
  virtual ~ChunkInfo() = default;

  std::atomic_bool m_done;
};

class ChunkTerrainInfo : public ChunkInfo {
 public:
  ChunkTerrainInfo(glm::ivec2 pos, int seed);

  void generateTerrainData();
  void applyTerrainDataToChunk(Chunk *chunk);

 private:
  Block m_blocks[CHUNK_VOLUME]{};
  int m_seed;
  glm::ivec2 m_pos;
};

class ChunkStructuresInfo : public ChunkInfo {
 public:
  explicit ChunkStructuresInfo(Chunk *chunks[9], int seed);

  void generateStructureData();

 private:
  TerrainGenerator m_terrainGenerator;
  glm::ivec2 m_pos;
};

class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(Chunk *chunks[9]);
  void generateMeshData();
  void applyMeshDataToMesh(Chunk *chunk);

 private:
  Chunk *m_chunks[9]{};
  std::vector<uint32_t> m_opaqueVertices;
  std::vector<uint32_t> m_transparentVertices;
  std::vector<unsigned int> m_opaqueIndices;
  std::vector<unsigned int> m_transparentIndices;
  glm::ivec2 m_pos{};
};

#endif //VOXEL_ENGINE_CHUNK_HPP
