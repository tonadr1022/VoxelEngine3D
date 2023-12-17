//
// Created by Tony Adriansen on 11/15/23.
//

#ifndef VOXEL_ENGINE_APPLICATION_HPP
#define VOXEL_ENGINE_APPLICATION_HPP

#include "world/World.hpp"
#include "camera/Camera.hpp"
#include "player/Player.hpp"
#include "input/Keyboard.hpp"
#include "EngineConfig.hpp"

class Application {
public:

    void run();

    static Application &getInstance();

private:
    const char* glsl_version{};

    explicit Application();

    GLFWwindow *window{};

    static void initOpenGL();

    void initGLFW();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    void processScrollInput(double xoffset, double yoffset);

    std::shared_ptr<World> world;

    static void compileShaders();

    static void loadTextures();
};

#endif //VOXEL_ENGINE_APPLICATION_HPP
