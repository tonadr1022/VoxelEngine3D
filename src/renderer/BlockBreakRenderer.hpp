//
// Created by Tony Adriansen on 12/3/23.
//

#ifndef VOXEL_ENGINE_BLOCKBREAKRENDERER_HPP
#define VOXEL_ENGINE_BLOCKBREAKRENDERER_HPP

#include "glm/vec3.hpp"
#include "../camera/Camera.hpp"

class BlockBreakRenderer {
 public:
  BlockBreakRenderer();
  void render(glm::vec3 blockPosition, const Camera &camera, int breakStage) const;

  unsigned int VAO{}, VBO{}, EBO{};

 private:
  unsigned int textureArrayId;

};

#endif //VOXEL_ENGINE_BLOCKBREAKRENDERER_HPP
