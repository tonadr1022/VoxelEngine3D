//
// Created by Tony Adriansen on 12/3/23.
//

#include "Keyboard.h"

void Keyboard::press(int key) {
    KeyPress& keyPress = keyStates[key];
    keyPress.current = true;
}

void Keyboard::release(int key) {
    KeyPress& keyPress = keyStates[key];
    keyPress.current = false;
}

bool Keyboard::isPressed(int key) {
    return keyStates[key].current;
}

bool Keyboard::isPressedThisFrame(int key) {
    return keyStates[key].current && !keyStates[key].previous;

}

bool Keyboard::isHeld(int key) {
    KeyPress& keyPress = keyStates[key];
    return keyPress.current && keyPress.previous;
}

void Keyboard::setWindow(GLFWwindow *pWindow) {
    Keyboard::window = pWindow;
}

void Keyboard::update() {
    for (auto& [key, keyPress] : keyStates) {
        keyPress.previous = keyPress.current;
    }
}

std::unordered_map<int, KeyPress> Keyboard::keyStates = std::unordered_map<int, KeyPress>();

GLFWwindow *Keyboard::window = nullptr;
