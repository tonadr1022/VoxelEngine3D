//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_RENDERER_H
#define VOXEL_ENGINE_RENDERER_H

#include "../Config.h"
#include "../camera/Camera.h"
#include "../shaders/Shader.h"
#include "../gui/CrossHair.h"

class Renderer {
public:
    explicit Renderer(GLFWwindow *window, Camera &camera);

    void highlightRender(glm::ivec3 blockPosition);

    void render();

    void renderCrossHair();

    void startRender(const std::shared_ptr<Shader>& shader);

private:
    std::shared_ptr<Shader> highlightShader;
    GLFWwindow *window;
    Camera &camera;
    CrossHair crossHair;


};


#endif //VOXEL_ENGINE_RENDERER_H
