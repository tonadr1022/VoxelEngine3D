//
// Created by Tony Adriansen on 1/6/2024.
//

#include "ChunkAlg.hpp"
#include "../block/BlockDB.hpp"

void ChunkAlg::generateLightData(ChunkStackArray chunks) {
  for (auto &chunk : chunks) {
    generateTorchlightData(chunk);
  }
  generateSunLightData(chunks);
}

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

void ChunkAlg::propagateTorchLightDirect(std::queue<LightNode> &torchlightQueue, World *world) {
  // while the queue is not empty, pop the front element and add it to the light level array
  while (!torchlightQueue.empty()) {
    LightNode node = torchlightQueue.front();
    torchlightQueue.pop();

    // For each adjacent neighbor for each face of the light source, check whether it has a lower light level
    // for any component. If so, replace the components that are lower in the neighbor with what is propagated
    // from the current light level
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      Block neighborBlock = world->getBlock(neighborPos);

      // if light cant pass don't add anything
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      const glm::ivec3 neighborLightLevel = world->getTorchLevel(neighborPos);
      glm::ivec3 unpackedNodeLightLevel = Utils::unpackLightLevel(node.lightLevel);
      if (neighborLightLevel.r < unpackedNodeLightLevel.r - 1 ||
          neighborLightLevel.g < unpackedNodeLightLevel.g - 1 ||
          neighborLightLevel.b < unpackedNodeLightLevel.b - 1) {
        const int newR = std::max(neighborLightLevel.r, unpackedNodeLightLevel.r - 1);
        const int newG = std::max(neighborLightLevel.g, unpackedNodeLightLevel.g - 1);
        const int newB = std::max(neighborLightLevel.b, unpackedNodeLightLevel.b - 1);
        const uint16_t newLightLevel = Utils::packLightLevel(newR, newG, newB);
        world->setTorchLight(neighborPos, newLightLevel, true);

        // if there is still light to propagate add to queue
        if (newR > 1 || newG > 1 || newB > 1) {
          torchlightQueue.emplace(neighborPos, newLightLevel);
        }
      }
    }
  }
}

void ChunkAlg::unpropagateSunLightDirect(std::queue<SunLightNodeWorld> &sunLightPlacementQueue,
                                         std::queue<SunLightNodeWorld> &sunlightRemovalQueue,
                                         World *world) {
  while (!sunlightRemovalQueue.empty()) {
    SunLightNodeWorld &node = sunlightRemovalQueue.front();
    sunlightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      const uint8_t neighborLightLevel = world->getSunlightLevel(neighborPos);

      if (neighborLightLevel == 0) continue;

      if (faceNum == 5 && node.lightLevel == 15) {
        world->setSunlight(neighborPos, 0, true);
        sunlightRemovalQueue.emplace(neighborPos, MAX_LIGHT_LEVEL);
      } else {
        if (neighborLightLevel >= node.lightLevel) {
          sunLightPlacementQueue.emplace(neighborPos, neighborLightLevel);
        } else {
          world->setSunlight(neighborPos, 0, true);
          sunlightRemovalQueue.emplace(neighborPos, neighborLightLevel);
        }
      }
    }
  }
}

void ChunkAlg::unpropagateSunLight(std::queue<SunLightNode> &sunLightPlacementQueue,
                                   std::queue<SunLightNode> &sunlightRemovalQueue,
                                   Chunk *chunk) {
  while (!sunlightRemovalQueue.empty()) {
    SunLightNode &node = sunlightRemovalQueue.front();
    sunlightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);
      const uint8_t neighborLightLevel = chunk->getSunlightLevelIncludingNeighborsOptimized(neighborPos);

      if (neighborLightLevel == 0) continue;

      if (faceNum == 5 && node.lightLevel == 15) {
        chunk->setSunlightIncludingNeighborsOptimized(neighborPos, 0);
        sunlightRemovalQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, MAX_LIGHT_LEVEL);
      } else {
        if (neighborLightLevel >= node.lightLevel) {
          sunLightPlacementQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighborLightLevel);
        } else {
          chunk->setSunlightIncludingNeighborsOptimized(neighborPos, 0);
          sunlightRemovalQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighborLightLevel);
        }
      }
    }
  }
}

void ChunkAlg::unpropagateTorchLightDirect(std::queue<LightNode> &torchLightPlacementQueue,
                                           std::queue<LightNode> &torchLightRemovalQueue,
                                           World *world) {
  while (!torchLightRemovalQueue.empty()) {
    LightNode node = torchLightRemovalQueue.front();
    torchLightRemovalQueue.pop();

    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      const uint16_t neighborLightLevel = world->getTorchLevelPacked(neighborPos);
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
      world->setTorchLight(neighborPos, 0, true);
      torchLightRemovalQueue.emplace(neighborPos, neighborLightLevel);
    }
  }
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

      const glm::ivec3
          neighborLightLevel = chunk->getTorchLevelIncludingNeighborsOptimized(neighborPos);
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

void ChunkAlg::generateSunLightData(ChunkStackArray &chunks) {
  int highestZ = 0;

  // find the highest z value of a block where light cannot pass
  bool done = false;
  for (int chunkIndex = CHUNKS_PER_STACK - 1; chunkIndex >= 0; chunkIndex--) {
    // if no non-air blocks, keep looking
    if (chunks[chunkIndex]->m_numNonAirBlocks == 0) continue;

    // if there's an air block in the chunk, iterate through the chunk until a block is found where light can't pass
    // get the highest z and break out of both loops
    for (int blockIndex = CHUNK_VOLUME - 1; blockIndex >= 0; blockIndex--) {
      if (!BlockDB::canLightPass(chunks[chunkIndex]->getBlockFromIndex(blockIndex))) {
        highestZ = chunkIndex * CHUNK_SIZE + (blockIndex / CHUNK_AREA);
        done = true;
        break;
      }
    }
    if (done) break;
  }

  // get the chunk index, chunk, and height in the chunk where the highest block is
  int highestZChunkIndex = highestZ / CHUNK_SIZE;
  Chunk *highestZChunk = chunks[highestZChunkIndex];
  int chunkZ = highestZ - highestZChunkIndex * CHUNK_SIZE;

  // fill everything above the highest block with max light level since nothing blocks it from the sun
  highestZChunk->fillSunlightWithZ(chunkZ + 1, MAX_LIGHT_LEVEL, true);

  // fill everything including the highest z layer and below with 0 since sunlight will be propagated (or not)
  highestZChunk->fillSunlightWithZ(chunkZ + 1, 0, false);


  // fill in chunks above highest block with max light level
  for (int chunkIndex = highestZChunkIndex + 1; chunkIndex < CHUNKS_PER_STACK; chunkIndex++) {
    chunks[chunkIndex]->fillSunlightWithZ(0, MAX_LIGHT_LEVEL, true);
  }

  for (int chunkIndex = 0; chunkIndex < highestZChunkIndex; chunkIndex++) {
    chunks[chunkIndex]->fillSunlightWithZ(0, 0, true);
  }
//  std::cout << highestZChunk->m_sunlightLevelsPtr.get()[0] << std::endl;

  std::queue<SunLightNode> sunlightQueue;
  glm::ivec3 pos;
  pos.z = highestZ; // chunk stack z since propagation function considers the whole stack

  // iterate through the layer that has the highest block. if light can pass, set that light level to max and add
  // it to the queue for propagation, otherwise, leave it since it's already 0
  for (pos.x = 0; pos.x < CHUNK_SIZE; pos.x++) {
    for (pos.y = 0; pos.y < CHUNK_SIZE; pos.y++) {
      if (BlockDB::canLightPass(highestZChunk->getBlock(pos.x, pos.y, chunkZ))) {
        sunlightQueue.emplace(pos.x, pos.y, pos.z, MAX_LIGHT_LEVEL);
        highestZChunk->setSunLightLevel({pos.x, pos.y, chunkZ}, MAX_LIGHT_LEVEL);
      }
    }
  }
  propagateSunLight(sunlightQueue, chunks);
}

void ChunkAlg::propagateSunLight(std::queue<SunLightNode> &sunLightQueue, Chunk *chunk) {
  while (!sunLightQueue.empty()) {
    SunLightNode &node = sunLightQueue.front();
    sunLightQueue.pop();
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);

      Block neighborBlock = chunk->getBlockIncludingNeighborsOptimized(neighborPos);
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      SunLightNode neighbor = SunLightNode(neighborPos.x, neighborPos.y, neighborPos.z, node.lightLevel);
      if (faceNum != 5 || node.lightLevel != MAX_LIGHT_LEVEL) {
        neighbor.lightLevel--;
      }
      int currNeighborSunlightLevel = chunk->getSunlightLevelIncludingNeighborsOptimized(neighborPos);
      if (currNeighborSunlightLevel < neighbor.lightLevel) {
        chunk->setSunlightIncludingNeighborsOptimized(neighborPos, neighbor.lightLevel);
        if (neighbor.lightLevel > 1) {
          sunLightQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighbor.lightLevel);
        }
      }
    }
  }
}

void ChunkAlg::propagateSunLight(std::queue<SunLightNode> &sunLightQueue, ChunkStackArray chunks) {
  auto setLightLevelInChunkStackIncludingNeighbors = [&](glm::ivec3 pos, uint8_t lightLevel) {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    chunks[chunkIndex]->setSunlightIncludingNeighborsOptimized(pos, lightLevel);
  };

  auto getBlockInChunkStackIncludingNeighbors = [&](glm::ivec3 pos) -> Block {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    return chunks[chunkIndex]->getBlockIncludingNeighborsOptimized(pos);
  };

  auto getSunlightLevelInChunkStackIncludingNeighbors = [&](glm::ivec3 pos) -> uint8_t {
    int chunkIndex = pos.z / CHUNK_SIZE;
    pos.z -= chunkIndex * CHUNK_SIZE;
    return chunks[chunkIndex]->getSunlightLevelIncludingNeighborsOptimized(pos);
  };

  while (!sunLightQueue.empty()) {
    SunLightNode &node = sunLightQueue.front();
    sunLightQueue.pop();
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace({node.x, node.y, node.z}, faceNum);
      if (neighborPos.z < 0 || neighborPos.z >= WORLD_HEIGHT_BLOCKS) continue;

      Block neighborBlock =
          getBlockInChunkStackIncludingNeighbors(neighborPos);
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      SunLightNode neighbor = SunLightNode(neighborPos.x, neighborPos.y, neighborPos.z, node.lightLevel);
      if (faceNum != 5 || node.lightLevel != MAX_LIGHT_LEVEL) {
        neighbor.lightLevel--;
      }
      int currNeighborSunlightLevel =
          getSunlightLevelInChunkStackIncludingNeighbors(neighborPos);
      if (currNeighborSunlightLevel < neighbor.lightLevel) {
        setLightLevelInChunkStackIncludingNeighbors(neighborPos, neighbor.lightLevel);
        if (neighbor.lightLevel > 1) {
          sunLightQueue.emplace(neighborPos.x, neighborPos.y, neighborPos.z, neighbor.lightLevel);
        }
      }
    }
  }
}

void ChunkAlg::propagateSunlightDirect(std::queue<SunLightNodeWorld> &sunlightQueue, World *world) {
  while (!sunlightQueue.empty()) {
    SunLightNodeWorld &node = sunlightQueue.front();
    sunlightQueue.pop();
    for (short faceNum = 0; faceNum < 6; faceNum++) {
      glm::ivec3 neighborPos = Utils::getNeighborPosFromFace(node.pos, faceNum);
      if (neighborPos.z < 0 || neighborPos.z >= WORLD_HEIGHT_BLOCKS) continue;

      Block neighborBlock = world->getBlock(neighborPos);
      if (!BlockDB::canLightPass(neighborBlock)) continue;

      SunLightNodeWorld neighbor = SunLightNodeWorld(neighborPos, node.lightLevel);
      if (faceNum != 5 || node.lightLevel != MAX_LIGHT_LEVEL) {
        neighbor.lightLevel--;
      }
      int currNeighborSunlightLevel = world->getSunlightLevel(neighborPos);
      if (currNeighborSunlightLevel < neighbor.lightLevel) {
        world->setSunlight(neighborPos, neighbor.lightLevel, true);
        if (neighbor.lightLevel > 1) {
          sunlightQueue.emplace(neighborPos, neighbor.lightLevel);
        }
      }
    }
  }
}