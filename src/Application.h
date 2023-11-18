//
// Created by Tony Adriansen on 11/15/23.
//

#ifndef VOXEL_ENGINE_APPLICATION_H
#define VOXEL_ENGINE_APPLICATION_H

#include "Config.h"
#include "camera/Camera.h"
#include "player/Player.h"

class Application {
public:
    explicit Application();

    void run();

private:
    static Application &getInstance() {
        static Application instance;
        return instance;
    }

    GLFWwindow *window{};
    Player player;

    static void initOpenGL();

    void initGlfwImGui();

//    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

//    static void mouseCallback(GLFWwindow *window, double xpos, double ypos);

//    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);


};


#endif //VOXEL_ENGINE_APPLICATION_H
