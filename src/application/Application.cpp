//
// Created by Tony Adriansen on 11/15/23.
//

#include "Application.hpp"
#include "../debug_gui/DebugGui.hpp"
#include "../shaders/ShaderManager.hpp"
#include "../resources/ResourceManager.hpp"

Application *Application::instancePtr = nullptr;

Application::Application() : m_window(DEFAULT_WINDOW_WIDTH,
                                    DEFAULT_WINDOW_HEIGHT,
                                    "Voxel Engine") {
  assert(instancePtr == nullptr && "Only one instance of Application allowed");
  instancePtr = this;
  m_window.lockCursor();

  ResourceManager::loadTextures();
  ShaderManager::compileShaders();

  m_world = std::make_unique<World>(m_renderer, 2, "default.wld");
}

Application::~Application() {
  instancePtr = nullptr;
}

void Application::run() {
  DebugGui debugGui(m_window.getContext(), m_window.getGlslVersion());

  double time = 0.0;
  const double dt = 1 / 60.0;
  double currentTime = m_window.getTime();

  while (m_running) {

    Keyboard::update();
    Mouse::update();

    m_window.pollEvents();

    m_window.beginFrame();

    m_world->update();

    double newTime = m_window.getTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;

    m_world->player.perFrameUpdate();
    while (frameTime > 0.0) {
      auto deltaTime = static_cast<float>(std::min(frameTime, dt));
      frameTime -= deltaTime;
      time += deltaTime;
      m_world->player.update(deltaTime);
    }

    debugGui.beginFrame();
    m_renderer.renderWorld(*m_world);
    m_world->renderDebugGui();
    debugGui.endFrame();

    m_window.swapBuffers();
  }
}

void Application::onKeyEvent(int key, int scancode, int action, int mods) {
  bool isPressed = action == GLFW_PRESS;
  bool isReleased = action == GLFW_RELEASE;
  if (isPressed && key == GLFW_KEY_ESCAPE) {
    m_running = false;
  }
  if (isPressed) {
    Keyboard::press(key);
  } else if (isReleased) {
    Keyboard::release(key);
  }
}

void Application::onScrollEvent(double xoffset, double yoffset) {
  m_world->player.processScrollInput(yoffset);
}

void Application::onMouseButtonEvent(int button, int action, int mods) {
  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }
  if (action == GLFW_PRESS) {
    Mouse::press(button);
  } else if (action == GLFW_RELEASE) {
    Mouse::release(button);
  }
}

void Application::onCursorPositionEvent(double xpos, double ypos) {
  static double lastX = xpos;
  static double lastY = ypos;

  double xOffset = xpos - lastX;
  double yOffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  if (!Keyboard::isPressed(GLFW_KEY_B)) {
    m_window.lockCursor();
    m_world->player.onCursorUpdate(xOffset, yOffset);
  } else {
    m_window.unlockCursor();
  }
}

void Application::onResizeEvent(int width, int height) {
  float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
  m_world->player.camera.updateProjectionMatrix(aspectRatio);
}


