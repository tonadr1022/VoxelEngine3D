//
// Created by Tony Adriansen on 12/3/23.
//

#ifndef VOXEL_ENGINE_KEYBOARD_HPP
#define VOXEL_ENGINE_KEYBOARD_HPP

#include "../EngineConfig.hpp"
#include "KeyPress.hpp"

class Keyboard {
 public:
  static void press(int key);
  static void release(int key);
  static bool isPressed(int key);
  static bool isPressedThisFrame(int key);
  static bool isHeld(int key);
  static void setWindow(GLFWwindow *pWindow);
  static void update();

 private:
  static std::unordered_map<int, KeyPress> keyStates;
  static GLFWwindow *window;
};

#endif //VOXEL_ENGINE_KEYBOARD_HPP
