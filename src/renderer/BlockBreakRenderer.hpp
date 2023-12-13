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
    void render(glm::vec3 blockPosition, Camera &camera, int breakStage) const;

    unsigned int VAO{}, VBO{}, EBO{};
    static constexpr int TEXTURE_Y_INDEX = 1;

private:

};


#endif //VOXEL_ENGINE_BLOCKBREAKRENDERER_HPP
