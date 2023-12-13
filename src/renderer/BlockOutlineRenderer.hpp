//
// Created by Tony Adriansen on 12/2/23.
//

#ifndef VOXEL_ENGINE_BLOCKOUTLINERENDERER_HPP
#define VOXEL_ENGINE_BLOCKOUTLINERENDERER_HPP

#include "../camera/Camera.hpp"

class BlockOutlineRenderer {
public:
    BlockOutlineRenderer();
    void render(glm::vec3 blockPosition, Camera &camera) const;

    unsigned int VAO, VBO, EBO;

private:

};


#endif //VOXEL_ENGINE_BLOCKOUTLINERENDERER_HPP
