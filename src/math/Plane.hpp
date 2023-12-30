//
// Created by Tony Adriansen on 12/30/2023.
//

#ifndef VOXEL_ENGINE_SRC_MATH_PLANE_HPP_
#define VOXEL_ENGINE_SRC_MATH_PLANE_HPP_

#include "../EngineConfig.hpp"

struct Plane {
  [[nodiscard]] inline float distanceToPoint(const glm::vec3 &point) const {
    return glm::dot(normal, point) + distance;
  }

  glm::vec3 normal;
  float distance;
};
#endif //VOXEL_ENGINE_SRC_MATH_PLANE_HPP_
