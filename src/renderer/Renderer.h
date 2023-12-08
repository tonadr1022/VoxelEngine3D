//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_RENDERER_H
#define VOXEL_ENGINE_RENDERER_H

#include "../shaders/Shader.h"
#include "../gui/CrossHair.h"
#include "BlockOutlineRenderer.h"
#include "BlockBreakRenderer.h"

class Renderer {
public:
    explicit Renderer(GLFWwindow *window);

    void render(Camera &camera);

    void renderCrossHair() const;
    void renderBlockOutline(Camera &camera, glm::ivec3 blockPosition);
    void renderBlockBreak(Camera &camera, glm::ivec3 blockPosition, float breakPercentage);

    void startRender(Camera &camera, const std::shared_ptr<Shader>& shader);

private:
    std::shared_ptr<Shader> highlightShader;
    BlockOutlineRenderer blockOutlineRenderer;
    BlockBreakRenderer blockBreakRenderer;

    GLFWwindow *window;
    CrossHair crossHair;



};


#endif //VOXEL_ENGINE_RENDERER_H
