//
// Created by Tony Adriansen on 12/4/23.
//

#ifndef VOXEL_ENGINE_DEBUGGUI_HPP
#define VOXEL_ENGINE_DEBUGGUI_HPP

#include "../world/World.hpp"

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


#endif //VOXEL_ENGINE_DEBUGGUI_HPP
