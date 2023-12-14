//
// Created by Tony Adriansen on 11/15/23.
//

#include "Application.hpp"
#include "shaders/Shader.hpp"
#include "world/World.hpp"
#include "resources/ResourceManager.hpp"
#include "world/block/BlockDB.hpp"
#include "shaders/ShaderManager.hpp"
#include "renderer/Renderer.hpp"
#include "debug_gui/DebugGui.hpp"
#include <iostream>

void Application::run() {
    initGLFW();
    initOpenGL();

    BlockDB::loadData("../resources/blocks/");
    loadTextures();
    compileShaders();

    Renderer renderer(window);
    world = std::make_shared<World>(window, renderer);

    DebugGui debugGui(window, glsl_version, world);

    double time = 0.0;
    const double dt = 1 / 60.0;
    double currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        Keyboard::update();
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world->update();

        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        world->player.perFrameUpdate(window);
        while (frameTime > 0.0) {
            auto deltaTime = static_cast<float>(std::min(frameTime, dt));
            frameTime -= deltaTime;
            time += deltaTime;
            world->player.update(window, deltaTime);
        }

        debugGui.update();

        // Rendering
        world->render();
        renderer.renderCrossHair();

        debugGui.render();


        glfwSwapBuffers(window);


    }
}

void Application::initOpenGL() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void Application::initGLFW() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return;
    }
    Keyboard::setWindow(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *windowArg, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glfwSetWindowUserPointer(window, this);
//    glfwSetMouseButtonCallback(window, mouseButtonCallback);
//    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }


}

Application::Application() {
    std::cout << "Application constructor" << std::endl;
}

void Application::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    Application &app = Application::getInstance();
    app.processScrollInput(xoffset, yoffset);
}

void Application::processScrollInput(double xoffset, double yoffset) {
    world->player.processScrollInput(yoffset);
}

void Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        Keyboard::press(key);
    } else if (action == GLFW_RELEASE) {
        Keyboard::release(key);
    }
}

void Application::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {

}

Application &Application::getInstance() {
    static Application instance;
    return instance;
}

void Application::compileShaders() {
    std::shared_ptr<Shader> chunkShader = std::make_shared<Shader>("../shaders/vertex.glsl",
                                                                   "../shaders/fragment.glsl");
    std::shared_ptr<Shader> outlineShader = std::make_shared<Shader>("../shaders/OutlineVert.glsl",
                                                                     "../shaders/OutlineFrag.glsl",
                                                                     "../shaders/OutlineGeom.glsl");
    std::shared_ptr<Shader> blockBreakShader = std::make_shared<Shader>(
            "../shaders/BlockBreakVert.glsl", "../shaders/BlockBreakFrag.glsl");
    std::shared_ptr<Shader> crossHairShader = std::make_shared<Shader>(
            "../shaders/CrossHairVert.glsl", "../shaders/CrossHairFrag.glsl");

    ShaderManager::addShader(chunkShader, "chunk");
    ShaderManager::addShader(outlineShader, "outline");
    ShaderManager::addShader(blockBreakShader, "blockBreak");
    ShaderManager::addShader(crossHairShader, "crosshair");

}

void Application::loadTextures() {
    ResourceManager::makeTexture("../resources/textures/default_pack_512.png", "texture_atlas",
                                 true);
}


