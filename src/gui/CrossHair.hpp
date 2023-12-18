//
// Created by Tony Adriansen on 11/29/23.
//

#ifndef VOXEL_ENGINE_CROSSHAIR_HPP
#define VOXEL_ENGINE_CROSSHAIR_HPP

#include "../EngineConfig.hpp"

class CrossHair {
public:
    CrossHair();
    unsigned int VAO, VBO;

    void render() const;
};


#endif //VOXEL_ENGINE_CROSSHAIR_HPP
