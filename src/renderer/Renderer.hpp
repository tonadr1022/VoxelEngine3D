//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_RENDERER_HPP
#define VOXEL_ENGINE_RENDERER_HPP

#include "../shaders/Shader.hpp"
#include "../gui/CrossHair.hpp"
#include "BlockOutlineRenderer.hpp"
#include "BlockBreakRenderer.hpp"

class Renderer {
public:
    explicit Renderer();

    void renderCrossHair() const;
    void renderBlockOutline(Camera &camera, glm::ivec3 blockPosition);
    void renderBlockBreak(Camera &camera, glm::ivec3 blockPosition, int breakStage);

private:
    BlockOutlineRenderer m_blockOutlineRenderer;
    BlockBreakRenderer m_blockBreakRenderer;
    CrossHair crossHair;

    void compileShaders();
    void loadTextures();



};


#endif //VOXEL_ENGINE_RENDERER_HPP
