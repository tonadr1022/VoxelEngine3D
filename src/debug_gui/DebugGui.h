//
// Created by Tony Adriansen on 12/4/23.
//

#ifndef VOXEL_ENGINE_DEBUGGUI_H
#define VOXEL_ENGINE_DEBUGGUI_H

#include "../world/World.h"

class DebugGui {
public:
    DebugGui(GLFWwindow *window, const char *glsl_version, std::shared_ptr<World> world);
    void render();
    void update();
    void destroy();

private:
    GLFWwindow *window;
    std::shared_ptr<World> world;

};


#endif //VOXEL_ENGINE_DEBUGGUI_H
