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
};

static inline int XYZ(int x, int y, int z) {
  return x + (y << 5) + (z << 10);
}

static inline int XYZ(glm::ivec3 pos) {
  return pos.x + (pos.y << 5) + (pos.z << 10) ;
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

  inline void setBlock(int x, int y, int z, Block block) {
    m_blocks[XYZ(x, y, z)] = block;
  }

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

class ChunkInfo {
 public:
  ChunkInfo() : m_done(false) {
  }
  virtual ~ChunkInfo() = default;

//  std::atomic_bool m_done;
bool m_done;
};

class ChunkTerrainInfo : public ChunkInfo {
 public:
  ChunkTerrainInfo(glm::ivec2 pos, int seed);

  void generateTerrainData();
  void applyTerrainDataToChunk(Chunk *(&chunk)[CHUNKS_PER_STACK]);

 private:
  Block m_blocks[CHUNK_VOLUME * CHUNKS_PER_STACK]{};
  int m_seed;
  glm::ivec2 m_pos;
};

class ChunkStructuresInfo : public ChunkInfo {
 public:
  explicit ChunkStructuresInfo(Chunk *chunks[27], int seed);

  void generateStructureData();

 private:
  TerrainGenerator m_terrainGenerator;
  glm::ivec3 m_pos;
};

class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(Chunk *chunks[27]);
  void generateMeshData();
  void applyMeshDataToMesh(Chunk *chunk);
  static void populateMeshInfoForMeshing(Block (&result)[CHUNK_MESH_INFO_SIZE], Chunk *(&chunks)[27]);

 private:
  Chunk *m_chunks[27]{};
  std::vector<uint32_t> m_opaqueVertices;
  std::vector<uint32_t> m_transparentVertices;
  std::vector<unsigned int> m_opaqueIndices;
  std::vector<unsigned int> m_transparentIndices;
};

#endif //VOXEL_ENGINE_CHUNK_HPP
