//
// Created by Tony Adriansen on 12/17/23.
//

#ifndef VOXEL_ENGINE_WINDOW_HPP
#define VOXEL_ENGINE_WINDOW_HPP

#include "../EngineConfig.hpp"

class Window {
 public:
  Window(int width, int height, const char *title);

  ~Window();

  [[nodiscard]] inline bool shouldClose() const {
    return glfwWindowShouldClose(window);
  };

  inline void close() const { glfwSetWindowShouldClose(window, true); }

  [[nodiscard]] inline GLFWwindow *getContext() { return window; }

  static void onKeyEvent(GLFWwindow *window,
                         int key,
                         int scancode,
                         int action,
                         int mods);

  static void onScrollEvent(GLFWwindow *window, double xoffset, double yoffset);

  static void onResizeEvent(GLFWwindow *window, int width, int height);

  static void onWindowCloseEvent(GLFWwindow *window);

  static void onMouseButtonEvent(GLFWwindow *window,
                                 int button,
                                 int action,
                                 int mods);

  static void onCursorPositionEvent(GLFWwindow *window,
                                    double xpos,
                                    double ypos);

  [[nodiscard]] glm::dvec2 getCursorPosition() const;

  [[nodiscard]] glm::dvec2 getCursorDelta() const;

  [[nodiscard]] glm::dvec2 getWindowCenter() const;

  [[nodiscard]] glm::ivec2 getWindowSize() const;

  void setCursorPosition(double xpos, double ypos);

  void lockCursor();

  void unlockCursor();

  void pollEvents();

  void centerCursor();

  void initializeCallbacks();

  void swapBuffers();

  inline const char *getGlslVersion() { return glsl_version; }

  [[nodiscard]] double getTime() const;

  void beginFrame() const;

 private:
  void initializeGlad();

  void initializeGlfw(int width, int height, const char *title);

  const char *glsl_version = "#version 330 core";
  GLFWwindow *window;
  glm::vec4 clearColor = {0,0,0,0};
//  glm::vec4 clearColor = {123.0f / 255.0f, 166.0f / 255.0f, 180.0f / 255.0f, 1.0f};
};

#endif //VOXEL_ENGINE_WINDOW_HPP
