//
// Created by Tony Adriansen on 11/15/23.
//

#ifndef VOXEL_ENGINE_APPLICATION_H
#define VOXEL_ENGINE_APPLICATION_H

#include "Config.h"
#include "camera/Camera.h"
#include "player/Player.h"
#include "world/World.h"
#include "input/Keyboard.h"

class Application {
public:

    void run();

    static Application &getInstance();

private:
    const char* glsl_version;

    explicit Application();

    GLFWwindow *window{};

    static void initOpenGL();

    void initGLFW();

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    void processScrollInput(double xoffset, double yoffset);

    std::shared_ptr<World> world;

    void compileShaders();

    void loadTextures();


//    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

//    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

//    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);


};


#endif //VOXEL_ENGINE_APPLICATION_H
