//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"

Chunk::Chunk(glm::ivec2 pos)
    : m_pos(pos),
      m_worldPos(pos * CHUNK_WIDTH),
      chunkMeshState(ChunkMeshState::UNBUILT),
      chunkState(ChunkState::UNGENERATED),
      m_boundingBox({glm::vec3(m_worldPos.x, m_worldPos.y, 0),
                     glm::vec3(m_worldPos.x + CHUNK_WIDTH, m_worldPos.y + CHUNK_WIDTH, CHUNK_HEIGHT)}) {}

Chunk::~Chunk() {
  m_transparentMesh.clearBuffers();
  m_transparentMesh.clearData();
  m_opaqueMesh.clearBuffers();
  m_opaqueMesh.clearData();
}

void Chunk::markDirty() {
  chunkMeshState = ChunkMeshState::UNBUILT;
  chunkState = ChunkState::CHANGED;
}

ChunkTerrainInfo::ChunkTerrainInfo(glm::ivec2 pos, int seed)
    : m_pos(pos), m_seed(seed) {
}

void ChunkTerrainInfo::generateTerrainData() {
  auto chunkWorldPos = glm::ivec2(m_pos.x * CHUNK_WIDTH, m_pos.y * CHUNK_WIDTH);
 TerrainGenerator::generateTerrain(chunkWorldPos, m_seed, m_blocks);
  m_done = true;
}

void ChunkTerrainInfo::applyTerrainDataToChunk(Chunk *chunk) {
  std::copy(m_blocks, m_blocks + CHUNK_VOLUME, chunk->m_blocks);
  chunk->chunkState = ChunkState::TERRAIN_GENERATED;
}

ChunkMeshInfo::ChunkMeshInfo(const Chunk &chunk0, const Chunk &chunk1, const Chunk &chunk2, const Chunk &chunk3,
                             const Chunk &chunk4, const Chunk &chunk5, const Chunk &chunk6, const Chunk &chunk7,
                             const Chunk &chunk8)
    : m_pos(chunk4.m_pos), m_chunk0(chunk0), m_chunk1(chunk1), m_chunk2(chunk2), m_chunk3(chunk3),
      m_chunk4(chunk4), m_chunk5(chunk5), m_chunk6(chunk6), m_chunk7(chunk7),
      m_chunk8(chunk8) {
}

void ChunkMeshInfo::process() {
  ChunkMeshBuilder meshBuilder(m_chunk0, m_chunk1, m_chunk2, m_chunk3,
                               m_chunk4, m_chunk5, m_chunk6, m_chunk7, m_chunk8);
  meshBuilder.constructMesh(m_opaqueVertices, m_opaqueIndices, m_transparentVertices, m_transparentIndices);
  m_done = true;
}

void ChunkMeshInfo::applyMeshDataToMesh(Chunk *chunk) {
  chunk->m_opaqueMesh.vertices = std::move(m_opaqueVertices);
  chunk->m_opaqueMesh.indices = std::move(m_opaqueIndices);
  chunk->m_transparentMesh.vertices = std::move(m_transparentVertices);
  chunk->m_transparentMesh.indices = std::move(m_transparentIndices);
  chunk->chunkMeshState = ChunkMeshState::BUILT;
}

ChunkStructuresInfo::ChunkStructuresInfo(Chunk &chunk0, Chunk &chunk1, Chunk &chunk2, Chunk &chunk3,
                                         Chunk &chunk4, Chunk &chunk5, Chunk &chunk6, Chunk &chunk7,
                                         Chunk &chunk8,
                                         int seed)
    : m_pos(chunk4.m_pos), m_terrainGenerator(chunk0, chunk1, chunk2, chunk3, chunk4, chunk5, chunk6,
                                              chunk7, chunk8, seed) {

}
void ChunkStructuresInfo::generateStructureData() {
  m_terrainGenerator.generateStructures();
  m_done = true;
}