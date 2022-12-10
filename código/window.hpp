#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;

  glm::ivec2 m_viewportSize{};

  Model m_model;
  Model ufo;
  Model planet;
  Model sun;

  glm::vec3 eye{0.0f, 0.0f, 0.0f};
  glm::vec3 at{0.0f, 0.0f, -1.0f};
  glm::vec3 up{0.0f, 1.0f, 0.0f};

  float lastX = 300, lastY = 300;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float sensitivity = 0.1f;
  glm::ivec2 mousePosition;

  struct Star {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  struct SpaceBody {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  SpaceBody m_ufo;
  SpaceBody m_planet;
  SpaceBody m_sun;

  std::array<Star, 1000> m_stars;

  float m_angle{};
  float m_angle2{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{30.0f};

  GLuint m_program{};
  GLuint m_program_2{};

  void randomizeStar(Star &star);
  bool control = false;
  void checkCollision();

  // Light and material properties
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};
};

#endif