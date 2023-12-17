//
// Created by Tony Adriansen on 12/14/23.
//

#ifndef VOXEL_ENGINE_IMAGE_HPP
#define VOXEL_ENGINE_IMAGE_HPP

#include "../EngineConfig.hpp"

struct Image {
    GLsizei width, height;
    std::vector<uint8_t> pixels;

    [[nodiscard]] Image subImage(glm::ivec2 offset, glm::ivec2 size) const;
};


#endif //VOXEL_ENGINE_IMAGE_HPP
