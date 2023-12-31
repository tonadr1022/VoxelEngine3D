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

ChunkMeshInfo::ChunkMeshInfo(Chunk *chunks[9]) {
  for (int i = 0; i < 9; i++) {
    m_chunks[i] = chunks[i];
  }
  m_pos = m_chunks[4]->m_pos;
}

void ChunkMeshInfo::process() {
  ChunkMeshBuilder meshBuilder(m_chunks);
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

ChunkStructuresInfo::ChunkStructuresInfo(Chunk *(chunks)[9], int seed) : m_terrainGenerator(chunks, seed), m_pos(chunks[4]->m_pos) {

}
void ChunkStructuresInfo::generateStructureData() {
  m_terrainGenerator.generateStructures();
  m_done = true;
}