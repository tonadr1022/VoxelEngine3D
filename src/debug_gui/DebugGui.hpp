//
// Created by Tony Adriansen on 12/4/23.
//

#ifndef VOXEL_ENGINE_DEBUGGUI_HPP
#define VOXEL_ENGINE_DEBUGGUI_HPP

#include "../world/World.hpp"

class DebugGui {
 public:
  DebugGui(GLFWwindow *context, const char *glsl_version);
  ~DebugGui();

  void beginFrame();
  void endFrame();

 private:
  GLFWwindow *context;

};

#endif //VOXEL_ENGINE_DEBUGGUI_HPP
