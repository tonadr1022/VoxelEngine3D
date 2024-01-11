//
// Created by Tony Adriansen on 1/6/2024.
//

#include "ChunkAlg.hpp"
#include "../block/BlockDB.hpp"

void ChunkAlg::generateTorchlightData(Chunk *chunk) {
  // torchlight
  std::queue<LightNode> torchlightQueue;
  // check for any light sources in the chunk and add them to the queue
  for (int blockIndex = 0; blockIndex < CHUNK_VOLUME; blockIndex++) {
    Block block = chunk->getBlockFromIndex(blockIndex);
    if (BlockDB::isLightSource(block)) {
      glm::ivec3 pos = XYZ_FROM_INDEX(blockIndex);
      uint16_t lightLevel = BlockDB::getpackedLightLevel(block);
      torchlightQueue.emplace(pos, lightLevel);
      chunk->setTorchLevelIncludingNeighborsOptimized(pos, lightLevel);
    }
  }

  propagateTorchLight(torchlightQueue, chunk);
  chunk->chunkState = ChunkState::FULLY_GENERATED;
}

void ChunkAlg::unpropagateTorchLight(std::queue<LightNode> &torchLightPlacementQueue,
                                     std::queue<LightNode> &torchLightRemovalQueue,
                                     Chunk *chunk) {
  while (!torchLightRemovalQueue.empty()) {
    LightNode node = torchLightRemovalQueue.front();
    torchLightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      const uint16_t neighborLightLevel = chunk->getTorchLevelPackedIncludingNeighborsOptimized(neighborPos);
      if (neighborLightLevel == 0) continue;

      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      glm::ivec3 unpackedNeighborLightLevel = Utils::unpackLightLevel(neighborLightLevel);
      glm::ivec3 newNeighborLightLevel = {0, 0, 0};
      bool place = false;
      if (unpackedNeighborLightLevel.r > 0 && unpackedNodeLightLevel.r > 0
          && unpackedNeighborLightLevel.r >= unpackedNodeLightLevel.r) {
        newNeighborLightLevel.r = unpackedNeighborLightLevel.r;
        place = true;
      }
      if (unpackedNeighborLightLevel.g > 0 && unpackedNodeLightLevel.g > 0
          && unpackedNeighborLightLevel.g >= unpackedNodeLightLevel.g) {
        newNeighborLightLevel.g = unpackedNeighborLightLevel.g;
        place = true;
      }
      if (unpackedNeighborLightLevel.b > 0 && unpackedNodeLightLevel.b > 0
          && unpackedNeighborLightLevel.b >= unpackedNodeLightLevel.b) {
        newNeighborLightLevel.b = unpackedNeighborLightLevel.b;
        place = true;
      }
      if (place) {
        torchLightPlacementQueue.emplace(neighborPos, Utils::packLightLevel(newNeighborLightLevel));
      }
      chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos, 0);
      torchLightRemovalQueue.emplace(neighborPos, neighborLightLevel);
    }
  }
}

void ChunkAlg::propagateTorchLight(std::queue<LightNode> &torchlightQueue, Chunk *chunk) {
  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();

    // For each adjacent neighbor for each face of the light source, check whether it has a lower light level
    // for any component. If so, replace the components that are lower in the neighbor with what is propagated
    // from the current light level
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);

      // if light cant pass don't add anything
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      const glm::ivec3 neighborLightLevel = chunk->getTorchLevelIncludingNeighborsOptimized(neighborPos);
      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      if (neighborLightLevel.r < unpackedNodeLightLevel.r - 1 ||
          neighborLightLevel.g < unpackedNodeLightLevel.g - 1 ||
          neighborLightLevel.b < unpackedNodeLightLevel.b - 1) {
        const int newR = std::max(neighborLightLevel.r, unpackedNodeLightLevel.r - 1);
        const int newG = std::max(neighborLightLevel.g, unpackedNodeLightLevel.g - 1);
        const int newB = std::max(neighborLightLevel.b, unpackedNodeLightLevel.b - 1);
        const uint16_t newLightLevel = Utils::packLightLevel(newR, newG, newB);
        chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos, newLightLevel);

        // if there is still light to propagate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.emplace(neighborPos, newLightLevel);
        }
      }
    }
  }
}

void ChunkAlg::generateSunLightData(Chunk *(&chunks)[CHUNKS_PER_STACK]) {
  for (int chunkIndex = CHUNKS_PER_STACK - 1; chunkIndex >= 0; chunkIndex--) {
    Chunk *chunk = chunks[chunkIndex];
    std::queue<SunLightNode> sunlightQueue;
    for (int blockIndex = 0; blockIndex < CHUNK_VOLUME; blockIndex++) {
      Block block = chunk->getBlockFromIndex(blockIndex);
      if (BlockDB::canLightPass(block)) {
        glm::ivec3 pos = XYZ_FROM_INDEX(blockIndex);
        sunlightQueue.emplace(pos, MAX_LIGHT_LEVEL);
        chunk->setSunLightLevel(pos, MAX_LIGHT_LEVEL);
      }
    }
    propagateSunLight(sunlightQueue, chunk);
  }
}

void ChunkAlg::propagateSunLight(std::queue<SunLightNode> &sunLightQueue, Chunk *chunk) {
  while (!sunLightQueue.empty()) {
    SunLightNode node = sunLightQueue.front();
    sunLightQueue.pop();
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      SunLightNode neighbor = SunLightNode(neighborPos, node.lightLevel);
      if (faceNum != 5 || node.lightLevel != MAX_LIGHT_LEVEL) {
        neighbor.lightLevel--;
      }
      int currNeighborSunlightLevel = chunk->getSunlightLevelIncludingNeighborsOptimized(neighborPos);
      if (neighbor.lightLevel < currNeighborSunlightLevel - 1) {
        int newLevel = std::max(currNeighborSunlightLevel, node.lightLevel - 1);
        chunk->setTorchLevelIncludingNeighborsOptimized(neighborPos,newLevel);
        if (newLevel > 1) {
          sunLightQueue.emplace(neighborPos, newLevel);
        }
      }
    }
    // bottom case
  }

}