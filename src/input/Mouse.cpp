//
// Created by Tony Adriansen on 12/17/23.
//

#include "Mouse.hpp"

void Mouse::press(int key) {
  KeyPress &keyPress = keyStates[key];
  keyPress.current = true;
}

void Mouse::release(int key) {
  KeyPress &keyPress = keyStates[key];
  keyPress.current = false;
}

bool Mouse::isPressed(int key) {
  return keyStates[key].current;
}

bool Mouse::isPressedThisFrame(int key) {
  return keyStates[key].current && !keyStates[key].previous;

}

bool Mouse::isHeld(int key) {
  KeyPress &keyPress = keyStates[key];
  return keyPress.current && keyPress.previous;
}

void Mouse::setWindow(GLFWwindow *pWindow) {
  window = pWindow;
}

void Mouse::update() {
  for (auto &[key, keyPress] : keyStates) {
    keyPress.previous = keyPress.current;
  }
}

std::unordered_map<int, KeyPress>
    Mouse::keyStates = std::unordered_map<int, KeyPress>();

GLFWwindow *Mouse::window = nullptr;