//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkManager.hpp"
#include "ChunkAlg.hpp"
#include "ChunkMeshBuilder.hpp"

Chunk::Chunk(glm::ivec2 pos) : m_pos(pos), m_worldPos(pos * CHUNK_WIDTH),
                               chunkMeshState(ChunkMeshState::UNBUILT), chunkState(ChunkState::UNGENERATED) {
}

Chunk::~Chunk() = default;

void Chunk::unload() {
  mesh.clearData();
  mesh.clearBuffers();
  chunkState = ChunkState::UNDEFINED;
  chunkMeshState = ChunkMeshState::UNBUILT;
}

void Chunk::setBlock(int x, int y, int z, Block block) {
  m_blocks[XYZ(x, y, z)] = block;
}

ChunkMesh &Chunk::getMesh() {
  return mesh;
}

void Chunk::markDirty() {
  chunkMeshState = ChunkMeshState::UNBUILT;
  chunkState = ChunkState::CHANGED;
}

ChunkLoadInfo::ChunkLoadInfo(glm::ivec2 pos, int seed)
    : m_pos(pos), m_seed(seed) {
}

void ChunkLoadInfo::process() {
  auto chunkWorldPos = glm::ivec2(m_pos.x * CHUNK_WIDTH, m_pos.y * CHUNK_WIDTH);

  FastNoiseSIMD *fastNoise = FastNoiseSIMD::NewFastNoiseSIMD();
  fastNoise->SetSeed(m_seed);
  fastNoise->SetFractalOctaves(4);
  fastNoise->SetFrequency(1.0f / 300.0f);
  float *heightMap = fastNoise->GetSimplexFractalSet(chunkWorldPos.x, chunkWorldPos.y, 0, CHUNK_WIDTH,
                                                     CHUNK_WIDTH, 1);

  int heights[CHUNK_AREA];
  int highest = 0;

  for (int i = 0; i < CHUNK_AREA; i++) {
    heights[i] = (int) floor(heightMap[i] * 64) + 100;
    highest = std::max(highest, heights[i]);
  }

  for (int z = 0; z <= highest; z++) {
    int heightMapIndex = 0;
    for (int x = 0; x < CHUNK_WIDTH; x++) {
      for (int y = 0; y < CHUNK_WIDTH; y++) {
        int height = heights[heightMapIndex];
        if (z < height) {
          m_blocks[XYZ(x, y, z)] = Block::DIRT;
        } else if (z == height) {
          m_blocks[XYZ(x, y, z)] = Block::GRASS;
        }
        heightMapIndex++;
      }
    }
  }
  m_done = true;
}

void ChunkLoadInfo::applyTerrain(Chunk *chunk) {
  std::copy(m_blocks, m_blocks + CHUNK_VOLUME, chunk->m_blocks);
  chunk->chunkState = ChunkState::TERRAIN_GENERATED;
}

ChunkMeshInfo::ChunkMeshInfo(const Chunk &chunk0, const Chunk &chunk1, const Chunk &chunk2, const Chunk &chunk3,
                             const Chunk &chunk4, const Chunk &chunk5, const Chunk &chunk6, const Chunk &chunk7,
                             const Chunk &chunk8)
    : m_pos(chunk4.m_pos),
      m_chunk_mesh_builder(chunk0, chunk1, chunk2, chunk3, chunk4, chunk5, chunk6, chunk7, chunk8) {

}

void ChunkMeshInfo::process() {
  m_chunk_mesh_builder.constructMesh(m_vertices, m_indices);
  m_done = true;
}

void ChunkMeshInfo::applyMesh(Chunk *chunk) {
  chunk->getMesh().vertices = std::move(m_vertices);
  chunk->getMesh().indices = std::move(m_indices);
  chunk->chunkMeshState = ChunkMeshState::BUILT;
}

ChunkGenerateStructuresInfo::ChunkGenerateStructuresInfo(Chunk &chunk0, Chunk &chunk1, Chunk &chunk2, Chunk &chunk3,
                                                         Chunk &chunk4, Chunk &chunk5, Chunk &chunk6, Chunk &chunk7,
                                                         Chunk &chunk8,
                                                         int seed)
    : m_pos(chunk4.m_pos), m_terrainGenerator(chunk0, chunk1, chunk2, chunk3, chunk4, chunk5, chunk6,
                                              chunk7, chunk8, seed) {

}
void ChunkGenerateStructuresInfo::process() {
  m_terrainGenerator.generateStructures();
  m_done = true;
}

void ChunkGenerateStructuresInfo::applyStructures(Chunk *chunk) {

}