//
// Created by Tony Adriansen on 12/30/2023.
//

#ifndef VOXEL_ENGINE_SRC_RENDERER_VIEWFRUSTUM_HPP_
#define VOXEL_ENGINE_SRC_RENDERER_VIEWFRUSTUM_HPP_

#include "../EngineConfig.hpp"
#include "../math/Plane.hpp"
#include "../math/AABB.hpp"

class ViewFrustum {
 public:
  ViewFrustum() = default;
  void updatePlanes(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix);

  [[nodiscard]] bool isBoxInFrustum(const AABB &boundingBox) const;

 private:
  std::array<Plane, 6> m_planes{};

};

#endif //VOXEL_ENGINE_SRC_RENDERER_VIEWFRUSTUM_HPP_
