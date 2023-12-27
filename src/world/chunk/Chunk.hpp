//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CHUNK_HPP
#define VOXEL_ENGINE_CHUNK_HPP

#include "ChunkMesh.hpp"
#include "../../AppConstants.hpp"
#include "../../EngineConfig.hpp"

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

  void unload();
  void setBlock(int x, int y, int z, Block block);
  ChunkMesh &getMesh();
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

  [[nodiscard]] inline bool hasNonAirBlockAt(int x, int y, int z) const {
    return m_blocks[XYZ(x, y, z)] != Block::AIR;
  }

  ChunkMeshState chunkMeshState;
  ChunkState chunkState;

  Block m_blocks[CHUNK_VOLUME]{};
  glm::ivec2 m_pos;
  glm::ivec2 m_worldPos;

 private:
  ChunkMesh mesh;
};

class ChunkInfo {
 public:
  ChunkInfo() : m_done(false) {}

  virtual void process() = 0;

  std::atomic_bool m_done;
};

class ChunkLoadInfo : public ChunkInfo {
 public:
  ChunkLoadInfo(glm::ivec2 pos, int seed);

  void process() override;
  void applyTerrain(Chunk *chunk);

 private:
  Block m_blocks[CHUNK_VOLUME]{};
  int m_seed;
  glm::ivec2 m_pos;
};

class ChunkGenerateStructuresInfo : public ChunkInfo {
 public:
  explicit ChunkGenerateStructuresInfo(glm::ivec2 pos, int seed);

  void process() override;
  void applyStructures(Chunk &chunk);

 private:
  Block m_blocks[CHUNK_VOLUME]{};
  int m_seed;
  glm::ivec2 m_pos;
};

class ChunkMeshInfo : public ChunkInfo {
 public:
  explicit ChunkMeshInfo(Chunk *(&chunks)[9]);

  void process() override;
  void applyMesh(Chunk *chunk);

 private:
  Block m_blocks[CHUNK_MESH_INFO_SIZE]{};
  std::vector<ChunkVertex> m_vertices;
  std::vector<unsigned int> m_indices;
  glm::ivec2 m_pos;
};

#endif //VOXEL_ENGINE_CHUNK_HPP
