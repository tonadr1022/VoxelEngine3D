//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_UTILS_HPP
#define VOXEL_ENGINE_UTILS_HPP

#include "../EngineConfig.hpp"
#include "../AppConstants.hpp"

namespace Utils {
inline static int positiveModulo(int value, int m) {
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
inline static int chunkNeighborOffset(int value) {
  return ((value & 0x80) != 0) ? -1 : ((value >> 5) & 1) ? 1 : 0;
}

// see neighbor array in world.hpp
inline static int getNeighborArrayIndex(int x, int y, int z) {
  return (x + 1) + 3 * ((y + 1) + 3 * (z + 1));
}

// if val == 33 return 1, if val ==
inline static int getRelativeIndex(int val) {
  return (val % CHUNK_SIZE + CHUNK_SIZE) % CHUNK_SIZE;
}

}

#endif //VOXEL_ENGINE_UTILS_HPP
