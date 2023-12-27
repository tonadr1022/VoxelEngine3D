//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_UTILS_HPP
#define VOXEL_ENGINE_UTILS_HPP

namespace Utils {
inline static int positiveModulo(int value, int m) {
  int mod = value % (int) m;
  if (mod < 0) {
    mod += m;
  }
  return mod;
}
}

#endif //VOXEL_ENGINE_UTILS_HPP
