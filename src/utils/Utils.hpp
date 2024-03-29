//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_UTILS_HPP
#define VOXEL_ENGINE_UTILS_HPP

#include "../EngineConfig.hpp"
#include "../AppConstants.hpp"

namespace Utils {
inline int positiveModulo(int value, int m) {
  int mod = value % (int) m;
  if (mod < 0) {
    mod += m;
  }
  return mod;
}

//int8_t chunkNeighborOffset(int8_t value) {
//  bool isNegative = (value & 0x80) != 0;
//  bool isAtLeast32 = (value >> 5) & 1;  // Check if at least 32
//  return isNegative ? -1 : isAtLeast32 ? 1 : 0;
//}

// if negative return -1, else if >= 32, return 1, else return 0;
inline int chunkNeighborOffset(int value) {
  return ((value & 0x80) != 0) ? -1 : ((value >> 5) & 1) ? 1 : 0;
}

// see neighbor array in world.hpp
inline int getNeighborArrayIndex(int x, int y, int z) {
  return (x + 1) + 3 * ((z + 1) + 3 * (y + 1));
}

inline int getChunkNeighborArrayIndexFromOutOfBoundsPos(const glm::ivec3 &pos) {
  return getNeighborArrayIndex(chunkNeighborOffset(pos.x), chunkNeighborOffset(pos.y), chunkNeighborOffset(pos.z));
}

// if val == 33 return 1, if val ==
inline int getLocalIndex(int val) {
  return (val & 31 + CHUNK_SIZE) & 31;
}

inline glm::ivec3 outOfBoundsPosToLocalPos(const glm::ivec3 outOfBoundsPos) {
  return {getLocalIndex(outOfBoundsPos.x), getLocalIndex(outOfBoundsPos.y), getLocalIndex(outOfBoundsPos.z)};
}

static inline uint16_t packLightLevel(const glm::ivec3 &level) {
  return static_cast<uint32_t>(level.r << 8 | level.g << 4 | level.b);
}
static inline uint16_t packLightLevel(int r, int g, int b) {
  return static_cast<uint32_t>(r << 8 | g << 4 | b);
}

static constexpr uint16_t RED_MASK = 0xF00;
static constexpr uint16_t GREEN_MASK = 0x0F0;
static constexpr uint16_t BLUE_MASK = 0x00F;

static inline glm::ivec3 unpackLightLevel(uint16_t level) {
  return {
      static_cast<int8_t>((level & RED_MASK) >> 8),
      static_cast<int8_t>((level & GREEN_MASK) >> 4),
      static_cast<int8_t>((level & BLUE_MASK)),
  };
}

inline glm::ivec3 getNeighborPosFromFace(glm::ivec3 pos, short faceNum) {
  pos[faceNum >> 1] += 1 - ((faceNum & 1) << 1);
  return pos;
}

}
#endif //VOXEL_ENGINE_UTILS_HPP
