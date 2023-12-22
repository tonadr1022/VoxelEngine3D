//
// Created by Tony Adriansen on 11/15/23.
//

#ifndef VOXEL_ENGINE_APPLICATION_HPP
#define VOXEL_ENGINE_APPLICATION_HPP

#include "../world/World.hpp"
#include "../camera/Camera.hpp"
#include "../player/Player.hpp"
#include "../input/Keyboard.hpp"
#include "../input/Mouse.hpp"
#include "../EngineConfig.hpp"
#include "Window.hpp"


class Application {
public:
    Application();

    ~Application();

    static Application &Instance() { return *instancePtr; }

    void run();

    void onKeyEvent(int key, int scancode, int action, int mods);

    void onMouseButtonEvent(int button, int action, int mods);

    void onCursorPositionEvent(double xpos, double ypos);

    void onScrollEvent(double xoffset, double yoffset);

    void onResizeEvent(int width, int height);

private:
    static Application *instancePtr;

    Window window;
    Renderer renderer;
    bool m_running = true;

    std::unique_ptr<World> m_world;

};

#endif //VOXEL_ENGINE_APPLICATION_HPP
