//
// Created by Tony Adriansen on 12/17/23.
//

#include "../input/Mouse.hpp"
#include "Window.hpp"
#include "../AppConstants.hpp"
#include "Application.hpp"

Window::Window(int width, int height, const char *title) : window(nullptr) {
  initializeGlfw(width, height, title);

  Keyboard::setWindow(window);
  Mouse::setWindow(window);

  initializeGlad();
  initializeCallbacks();
}

Window::~Window() {
  glfwTerminate();
}

void Window::onKeyEvent(GLFWwindow *window,
                        int key,
                        int scancode,
                        int action,
                        int mods) {
  Application::Instance().onKeyEvent(key, scancode, action, mods);
}

void Window::onMouseButtonEvent(GLFWwindow *window,
                                int button,
                                int action,
                                int mods) {
  Application::Instance().onMouseButtonEvent(button, action, mods);
}

void Window::onCursorPositionEvent(GLFWwindow *window,
                                   double xpos,
                                   double ypos) {
  Application::Instance().onCursorPositionEvent(xpos, ypos);
}

void Window::onScrollEvent(GLFWwindow *window, double xoffset, double yoffset) {
  Application::Instance().onScrollEvent(xoffset, yoffset);
}

void Window::onResizeEvent(GLFWwindow *window, int width, int height) {
  Application::Instance().onResizeEvent(width, height);
}

void Window::initializeCallbacks() {
  glfwSetFramebufferSizeCallback(window,
                                 [](GLFWwindow *windowArg,
                                    int width,
                                    int height) {
                                   glViewport(0, 0, width, height);
                                 });
  glfwSetWindowUserPointer(window, this);
  glfwSetMouseButtonCallback(window, onMouseButtonEvent);
  glfwSetKeyCallback(window, onKeyEvent);
  glfwSetCursorPosCallback(window, onCursorPositionEvent);
  glfwSetScrollCallback(window, onScrollEvent);
  glfwSetWindowSizeCallback(window, onResizeEvent);
}

void Window::lockCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::unlockCursor() {
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::setCursorPosition(double xpos, double ypos) {
  glfwSetCursorPos(window, xpos, ypos);
}

glm::dvec2 Window::getCursorPosition() const {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);
  return {xpos, ypos};
}

void Window::swapBuffers() {
  glfwSwapBuffers(window);
}

void Window::centerCursor() {
  // get window size
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  glfwSetCursorPos(window, static_cast<float>(windowWidth) / 2.0f,
                   static_cast<float>(windowHeight) / 2.0f);
}

glm::dvec2 Window::getWindowCenter() const {
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  return {static_cast<float>(windowWidth) / 2.0f,
          static_cast<float>(windowHeight) / 2.0f};
}

glm::dvec2 Window::getCursorDelta() const {
  glm::dvec2 cursorPosition = getCursorPosition();
  glm::dvec2 windowCenter = getWindowCenter();
  return cursorPosition - windowCenter;
}

void Window::pollEvents() {
  glfwPollEvents();
}

double Window::getTime() const {
  return glfwGetTime();
}

void Window::beginFrame() const {
  glm::ivec2 windowSize = getWindowSize();
  glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

glm::ivec2 Window::getWindowSize() const {
  int width, height;
  glfwGetWindowSize(window, &width, &height);
  return {width, height};
}

void Window::initializeGlad() {
  if (!gladLoadGL()) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return;
  }
}

void Window::initializeGlfw(int width, int height, const char *title) {
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
  glsl_version = "#version 330 core";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
  // GL 3.0 + GLSL 130
  glsl_version = "#version 330 core";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(window);
}

