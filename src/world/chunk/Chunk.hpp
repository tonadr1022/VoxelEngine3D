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
  return z * CHUNK_AREA + y * CHUNK_WIDTH + x;
}

static inline int XYZ(glm::ivec3 pos) {
  return pos.z * CHUNK_AREA + pos.y * CHUNK_WIDTH + pos.x;
}

static inline int XY(int x, int y) {
  return x + y * CHUNK_WIDTH;
}

static inline int XY(glm::ivec2 &pos) {
  return pos.x + pos.y * CHUNK_WIDTH;
}

static inline int MESH_XYZ(int x, int y, int z) {
  return (z + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH * CHUNK_MESH_INFO_CHUNK_WIDTH +
      (y + 1) * CHUNK_MESH_INFO_CHUNK_WIDTH + (x + 1);
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

  ChunkMeshState chunkMeshState;
  ChunkState chunkState;

  Block m_blocks[CHUNK_VOLUME]{};
  glm::ivec2 m_pos;
  glm::ivec2 m_worldPos;

  /**
   * @brief index 0 is opaque, 1 is transparent, 2 is other (floral etc)
   */
  ChunkMesh m_opaqueMesh;
  ChunkMesh m_transparentMesh;
};

class ChunkInfo {
 public:
  ChunkInfo() : m_done(false) {
  }

  virtual ~ChunkInfo() = default;


  std::atomic_bool m_done;
};

class ChunkLoadInfo : public ChunkInfo {
 public:
  ChunkLoadInfo(glm::ivec2 pos, int seed);

  Scope<std::array<int, CHUNK_AREA>> process();
  void applyTerrain(Chunk *chunk);

 private:
  Block m_blocks[CHUNK_VOLUME]{};
  int m_seed;
  glm::ivec2 m_pos;
};

class ChunkGenerateStructuresInfo : public ChunkInfo {
 public:
  explicit ChunkGenerateStructuresInfo(Chunk &chunk0,
                                       Chunk &chunk1,
                                       Chunk &chunk2,
                                       Chunk &chunk3,
                                       Chunk &chunk4,
                                       Chunk &chunk5,
                                       Chunk &chunk6,
                                       Chunk &chunk7,
                                       Chunk &chunk8, int seed);

  void process(const std::array<int, CHUNK_AREA> &heightMap);
  void applyStructures(Chunk *chunk);

 private:
  TerrainGenerator m_terrainGenerator;
  glm::ivec2 m_pos;
};

class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(const Chunk &chunk0, const Chunk &chunk1,
                         const Chunk &chunk2, const Chunk &chunk3,
                         const Chunk &chunk4, const Chunk &chunk5,
                         const Chunk &chunk6, const Chunk &chunk7,
                         const Chunk &chunk8);

  void process();
  void applyMesh(Chunk *chunk);

 private:
  const Chunk &m_chunk0;
  const Chunk &m_chunk1;
  const Chunk &m_chunk2;
  const Chunk &m_chunk3;
  const Chunk &m_chunk4;
  const Chunk &m_chunk5;
  const Chunk &m_chunk6;
  const Chunk &m_chunk7;
  const Chunk &m_chunk8;

  std::vector<uint32_t> m_opaqueVertices;
  std::vector<uint32_t> m_transparentVertices;
  std::vector<unsigned int> m_opaqueIndices;
  std::vector<unsigned int> m_transparentIndices;
  glm::ivec2 m_pos;
};

#endif //VOXEL_ENGINE_CHUNK_HPP
