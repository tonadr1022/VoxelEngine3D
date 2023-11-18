//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_RENDERER_H
#define VOXEL_ENGINE_RENDERER_H

#include "../Config.h"
#include "../camera/Camera.h"

class Renderer {
public:
    explicit Renderer(GLFWwindow *window, Camera &camera);

    void render();

private:
    GLFWwindow *window;
    Camera &camera;


};


#endif //VOXEL_ENGINE_RENDERER_H
