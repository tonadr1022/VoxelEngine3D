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

void Chunk::setTorchLevelIncludingNeighborsOptimized(glm::ivec3 pos, uint16_t lightLevelPacked, bool setFlag) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setTorchLevel(localPos, lightLevelPacked);
      if (setFlag) neighborChunk->m_flagForRemesh = true;
    }
  } else {
    setTorchLevel(pos, lightLevelPacked);
    if (setFlag) m_flagForRemesh = true;
  }
}

void Chunk::setBlockIncludingNeighborsOptimized(glm::ivec3 pos, Block block, bool setFlag) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setBlock(localPos, block);
      if (setFlag) neighborChunk->m_flagForRemesh = true;
    }
  } else {
    setBlock(pos, block);
    if (setFlag) m_flagForRemesh = true;
  }
}

void Chunk::setSunlightIncludingNeighborsOptimized(const glm::ivec3 &pos, uint8_t lightLevel, bool setFlag) {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      neighborChunk->setSunLightLevel(localPos, lightLevel);
      if (setFlag) neighborChunk->m_flagForRemesh = true;
    }
  } else {
    setSunLightLevel(pos, lightLevel);
    if (setFlag) m_flagForRemesh = true;
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

glm::ivec3 Chunk::getTorchLevelIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getTorchLevel(localPos);
    }
  } else {
    return getTorchLevel(pos);
  }
}

void Chunk::allocateTorchLightLevels() {
  if (!m_torchLightLevelsPtr) {
    m_torchLightLevelsPtr = std::make_unique<uint16_t[]>(CHUNK_VOLUME);
    std::fill(m_torchLightLevelsPtr.get(), m_torchLightLevelsPtr.get() + CHUNK_VOLUME, 0);
  }
}
uint16_t Chunk::getTorchLevelPackedIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getTorchLevelPacked(localPos);
    }
    return 0;
  } else {
    return getTorchLevelPacked(pos);
  }
}

uint8_t Chunk::getSunlightLevelIncludingNeighborsOptimized(glm::ivec3 pos) const {
  if (isPosOutOfChunkBounds(pos)) {
    int neighborArrayIndex = Utils::getChunkNeighborArrayIndexFromOutOfBoundsPos(pos);
    const Chunk *neighborChunk = m_neighborChunks[neighborArrayIndex];
    if (neighborChunk) {
      glm::ivec3 localPos = Utils::outOfBoundsPosToLocalPos(pos);
      return neighborChunk->getSunLightLevel(localPos);
    }
    return 0;
  } else {
    return getSunLightLevel(pos);
  }
}

void Chunk::allocateSunLightLevels() {
  if (!m_sunlightLevelsPtr) {
    m_sunlightLevelsPtr = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
    std::fill(m_sunlightLevelsPtr.get(), m_sunlightLevelsPtr.get() + CHUNK_VOLUME, 0);
  }
}

void Chunk::fillSunlightWithZ(int z, uint8_t lightLevel, bool atOrAbove) {
  if (!m_sunlightLevelsPtr) {
    m_sunlightLevelsPtr = std::make_unique<uint8_t[]>(CHUNK_VOLUME);
    if (atOrAbove) {
      std::fill(m_sunlightLevelsPtr.get() + CHUNK_AREA * z, m_sunlightLevelsPtr.get() + CHUNK_VOLUME, lightLevel);
    } else {
      std::fill(m_sunlightLevelsPtr.get(), m_sunlightLevelsPtr.get() + CHUNK_AREA * z, lightLevel);
    }
  }
}

