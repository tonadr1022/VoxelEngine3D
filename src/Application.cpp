//
// Created by Tony Adriansen on 11/15/23.
//

#include "Application.h"
#include "shaders/Shader.h"
#include "world/World.h"
#include "resources/ResourceManager.h"
#include "world/block/BlockDB.h"
#include "shaders/ShaderManager.h"
#include "renderer/Renderer.h"


void Application::run() {
    BlockDB::loadData("../resources/blocks/");
    ResourceManager::makeTexture("../resources/textures/default_texture.png", "texture_atlas", true);
    std::shared_ptr<Shader> chunkShader = std::make_shared<Shader>("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    std::shared_ptr<Shader> highlightShader = std::make_shared<Shader>("../shaders/HighlightVert.glsl", "../shaders/HighlightFrag.glsl");
    ShaderManager::addShader(chunkShader, "chunk");
    ShaderManager::addShader(highlightShader, "highlight");
    World world(window, player);
    Renderer renderer(window, player.camera);

    std::shared_ptr<Shader> crossHairShader = std::make_shared<Shader>("../shaders/CrossHairVert.glsl", "../shaders/CrossHairFrag.glsl");
    ShaderManager::addShader(crossHairShader, "crosshair");
    glm::mat4 crossHairModel = glm::mat4(1.0f);
    crossHairModel = glm::scale(crossHairModel, glm::vec3(0.03f));
    glm::vec3 crossHairColor = glm::vec3(1.0f, 1.0f, 1.0f);
    ShaderManager::getShader("crosshair")->use();
    ShaderManager::getShader("crosshair")->setMat4("u_Model", crossHairModel);
    ShaderManager::getShader("crosshair")->setVec3("u_Color", crossHairColor);

    double time = 0.0;
    const double dt = 1 / 60.0;
    double currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiIO &io = ImGui::GetIO();
        {
            ImGui::Begin("Testing");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                        io.Framerate);
            glm::vec3& cameraPos = player.getPosition();
            ImGui::Text("Camera Position  %.2f x  %.2f y %.2f z",
                        cameraPos.x, cameraPos.y, cameraPos.z);
            ImGui::SliderInt("Render Distance", &world.renderDistance, 1, 32);
            ImGui::End();
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world.update();
        world.render();
        renderer.renderCrossHair();

        double newTime = glfwGetTime();
        double frameTime = newTime - currentTime;
        currentTime = newTime;

        while (frameTime > 0.0) {
            float deltaTime = std::min(frameTime, dt);
            frameTime -= deltaTime;
            time += deltaTime;
            player.update(window, deltaTime);
        }

//        player.update(window, deltaTime);
//        world.update();


        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Application::initOpenGL() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//    glEnable(GL_BLEND);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
}

void Application::initGlfwImGui() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *windowArg, int width, int height) {
        glViewport(0, 0, width, height);
    });
    glfwSetWindowUserPointer(window, this);
//    glfwSetMouseButtonCallback(window, mouseButtonCallback);
//    glfwSetCursorPosCallback(window, mouseCallback);
//    glfwSetKeyCallback(window, keyCallback);


    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

//void Application::mouseCallback(GLFWwindow *window, double xpos, double ypos) {
//    Application &app = getInstance();
//    bool ImGuiWantMouse = ImGui::GetIO().WantCaptureMouse;
//    if (!ImGuiWantMouse && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
//        app.player.processMouseInput(xpos, ypos);
//    }
//}

//void Application::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
//}

Application::Application() : player() {
    initGlfwImGui();
    initOpenGL();
}

//void Application::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//    Application &app = getInstance();
//    app.player.update(window);
//}


