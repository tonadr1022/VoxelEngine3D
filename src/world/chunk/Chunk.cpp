//
// Created by Tony Adriansen on 11/16/23.
//

#include "Chunk.hpp"
#include "ChunkMeshBuilder.hpp"
#include "../block/BlockDB.hpp"

Chunk::Chunk(glm::ivec3 pos)
    : m_pos(pos),
      m_worldPos(pos * CHUNK_SIZE),
      chunkMeshState(ChunkMeshState::UNBUILT),
      chunkState(ChunkState::UNGENERATED),
      m_boundingBox({m_worldPos, m_worldPos + CHUNK_SIZE}) {}

Chunk::~Chunk() {
  m_transparentMesh.clearBuffers();
  m_transparentMesh.clearData();
  m_opaqueMesh.clearBuffers();
  m_opaqueMesh.clearData();
}

void Chunk::setLightLevelIncludingNeighborsOptimized(const glm::ivec3 pos, uint16_t lightLevelPacked) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setLightLevel(localPos, lightLevelPacked);
    }
  } else {
    setLightLevel(pos, lightLevelPacked);
  }
}

void Chunk::setBlockIncludingNeighborsOptimized(glm::ivec3 pos, Block block) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setBlock(localPos, block);
    }
  } else {
    setBlock(pos, block);
  }
}

Block Chunk::getBlockIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getBlock(localPos);
    }
  } else {
    return getBlock(pos);
  }
}

glm::ivec3 Chunk::getLightLevelIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getLightLevel(localPos);
    }
  } else {
    return getLightLevel(pos);
  }
}

void Chunk::allocateTorchLightLevels() {
  if (!m_torchLightLevelsPtr) {
    m_torchLightLevelsPtr = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
    std::fill(m_torchLightLevelsPtr.get(), m_torchLightLevelsPtr.get() + CHUNK_VOLUME, 0);
  }
}
uint16_t Chunk::getLightLevelPackedIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getLightLevelPacked(localPos);
    }
  } else {
    return getLightLevelPacked(pos);
  }
}

