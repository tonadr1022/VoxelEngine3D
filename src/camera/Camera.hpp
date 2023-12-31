//
// Created by Tony Adriansen on 11/16/23.
//

#ifndef VOXEL_ENGINE_CAMERA_HPP
#define VOXEL_ENGINE_CAMERA_HPP

#include "../physics/Ray.hpp"
#include "../EngineConfig.hpp"
#include "../AppConstants.hpp"

class Camera {
 public:
  explicit Camera();

  void onCursorUpdate(double xOffset, double yOffset);
  void setPosition(glm::vec3 position);
  void updateViewMatrix();
  void updateProjectionMatrix(float aspectRatio);
  [[nodiscard]] inline const glm::vec3 &getPosition() const { return m_position; }
  [[nodiscard]] inline const glm::mat4 &getViewMatrix() const { return m_viewMatrix; }
  [[nodiscard]] inline const glm::mat4 &getProjectionMatrix() const { return m_projectionMatrix; }
  [[nodiscard]] inline const glm::vec3 &getFront() const { return m_front; }
  static const inline glm::vec3 &getGlobalUp() { return const_cast<glm::vec3 &>(globalUp); }

 private:
  void updateCameraVectors();

  float m_fov = 70.0f;
  float m_mouseSensitivity = 0.1f;

  constexpr static float FAR_PLANE = 3000.0f;
  constexpr static float NEAR_PLANE = 0.1f;
  constexpr static glm::vec3 globalUp = glm::vec3(0.0f, 0.0f, 1.0f);

  glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);

  float m_yaw_deg = -90.0f;
  float m_pitch_deg = 0.0f;

  glm::vec3 m_front = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::vec3 m_up = globalUp;
  glm::vec3 m_right = glm::normalize(glm::cross(m_front, m_up));

  glm::mat4 m_viewMatrix = glm::mat4(1.0f);
  glm::mat4 m_projectionMatrix = glm::perspective(
      glm::radians(m_fov),
      static_cast<float>(DEFAULT_WINDOW_WIDTH)
          / static_cast<float>(DEFAULT_WINDOW_HEIGHT),
      NEAR_PLANE, FAR_PLANE);
};

#endif //VOXEL_ENGINE_CAMERA_HPP
