#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event) {

  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  float speed = 5.0f * deltaTime;
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {

    if (event.key.keysym.sym == SDLK_a)
      eye -= glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_d)
      eye += glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_w)
      eye += speed * at;
    if (event.key.keysym.sym == SDLK_s)
      eye -= speed * at;
    if (event.key.keysym.sym == SDLK_r)
      eye += speed * up;
    if (event.key.keysym.sym == SDLK_f)
      eye -= speed * up;
  }

  if (event.type == SDL_MOUSEWHEEL) {
    if (event.wheel.y > 0 && m_FOV > 15.0f)
      m_FOV -= 1.0f;
    if (event.wheel.y < 0 && m_FOV < 70.0f)
      m_FOV += 1.0f;
  }

  if (event.type == SDL_MOUSEMOTION) {
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "texture.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "texture.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_program_2 =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_model.loadObj(assetsPath + "star.obj");
  m_model.setupVAO(m_program_2);

  ufo.loadDiffuseTexture(assetsPath + "13884_Diffuse.jpg");
  ufo.loadObj(assetsPath + "13884_UFO_Saucer_v1_l2.obj");
  ufo.setupVAO(m_program);

  planet.loadDiffuseTexture(assetsPath + "Diffuse_2K.png");
  planet.loadObj(assetsPath + "Moon_2K.obj");
  planet.setupVAO(m_program);

  sun.loadDiffuseTexture(assetsPath + "13913_Sun_diff.jpg");
  sun.loadObj(assetsPath + "13913_Sun_v2_l3.obj");
  sun.setupVAO(m_program);

  // Use material properties from the loaded model
  m_Ka = ufo.getKa();
  m_Kd = ufo.getKd();
  m_Ks = ufo.getKs();
  m_shininess = ufo.getShininess();

  // Camera at (0,0,0) and looking towards the negative z

  m_viewMatrix = glm::lookAt(eye, eye + at, up);

  // Setup stars
  for (auto &star : m_stars) {
    randomizeStar(star);
  }

  m_planet.m_position = glm::vec3(20.0f, 0.0f, -100.0f);
  m_planet.m_rotationAxis = glm::vec3(0.0f, 1.0f, 1.0f);

  m_sun.m_position = glm::vec3(0.0f, 0.0f, -90.0f);
  m_sun.m_rotationAxis = glm::vec3(0.0f, 1.0f, 1.0f);

  m_ufo.m_position = glm::vec3(10.0f, 1.0f, -30.0f);
  m_ufo.m_rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
}

void Window::randomizeStar(Star &star) {

  std::uniform_real_distribution<float> distPosXY(-50.0f, 50.0f);
  std::uniform_real_distribution<float> distPosZ(-150.0f, -50.0f);
  star.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  // Random rotation axis
  star.m_rotationAxis = glm::sphericalRand(1.0f);
}

void Window::onUpdate() {
  // Increase angle by 90 degrees per second
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  m_angle2 = glm::wrapAngle(m_angle2 + glm::radians(2.0f) * deltaTime);

  float xoffset = mousePosition.x - lastX;
  float yoffset = lastY - mousePosition.y;
  lastX = mousePosition.x;
  lastY = mousePosition.y;

  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  at = glm::normalize(direction);

  m_viewMatrix = glm::lookAt(eye, eye + at, up);

  m_ufo.m_position -= deltaTime * glm::vec3{0.3f, 0.03f, -0.2f};

  m_planet.m_position.x = m_sun.m_position.x +
                          (50.0f - m_sun.m_position.x) * cos(m_angle2) -
                          (-80.0f - m_sun.m_position.z) * sin(m_angle2);

  m_planet.m_position.z = m_sun.m_position.z +
                          (50.0f - m_sun.m_position.x) * sin(m_angle2) +
                          (-80.0f - m_sun.m_position.z) * cos(m_angle2);

  checkCollision();
}

void Window::onPaint() {

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};
  m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 250.0f);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const normalMatrixLoc{
      abcg::glGetUniformLocation(m_program, "normalMatrix")};
  auto const lightDirLoc{
      abcg::glGetUniformLocation(m_program, "lightDirWorldSpace")};
  auto const shininessLoc{abcg::glGetUniformLocation(m_program, "shininess")};
  auto const IaLoc{abcg::glGetUniformLocation(m_program, "Ia")};
  auto const IdLoc{abcg::glGetUniformLocation(m_program, "Id")};
  auto const IsLoc{abcg::glGetUniformLocation(m_program, "Is")};
  auto const KaLoc{abcg::glGetUniformLocation(m_program, "Ka")};
  auto const KdLoc{abcg::glGetUniformLocation(m_program, "Kd")};
  auto const KsLoc{abcg::glGetUniformLocation(m_program, "Ks")};
  auto const diffuseTexLoc{abcg::glGetUniformLocation(m_program, "diffuseTex")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform1i(diffuseTexLoc, 0);

  auto const lightDirRotated{glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

  abcg::glUniform4fv(lightDirLoc, 1, &lightDirRotated.x);
  abcg::glUniform4fv(IaLoc, 1, &m_Ia.x);
  abcg::glUniform4fv(IdLoc, 1, &m_Id.x);
  abcg::glUniform4fv(IsLoc, 1, &m_Is.x);

  // Set uniform variables for the current model
  glm::mat4 modelMatrixUfo{1.0f};

  auto const modelViewMatrix{glm::mat3(m_viewMatrix * modelMatrixUfo)};
  auto const normalMatrix{glm::inverseTranspose(modelViewMatrix)};
  abcg::glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, &normalMatrix[0][0]);

  abcg::glUniform4fv(KaLoc, 1, &m_Ka.x);
  abcg::glUniform4fv(KdLoc, 1, &m_Kd.x);
  abcg::glUniform4fv(KsLoc, 1, &m_Ks.x);
  abcg::glUniform1f(shininessLoc, m_shininess);

  // Ufo
  modelMatrixUfo = glm::translate(modelMatrixUfo, m_ufo.m_position);
  modelMatrixUfo = glm::scale(modelMatrixUfo, glm::vec3(0.8f));
  modelMatrixUfo =
      glm::rotate(modelMatrixUfo, 80.0f, glm::vec3{1.0f, 0.0f, 0.0f});
  modelMatrixUfo = glm::rotate(modelMatrixUfo, m_angle, m_ufo.m_rotationAxis);

  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixUfo[0][0]);

  ufo.render();

  for (auto &star : m_stars) {
    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, star.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, star.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_model.renderAlt();
  }

  // planet
  glm::mat4 modelMatrixPlanet{1.0f};
  modelMatrixPlanet = glm::translate(modelMatrixPlanet, m_planet.m_position);
  modelMatrixPlanet = glm::scale(modelMatrixPlanet, glm::vec3(14.0f));
  modelMatrixPlanet =
      glm::rotate(modelMatrixPlanet, m_angle2, m_planet.m_rotationAxis);

  //  Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE,
                           &modelMatrixPlanet[0][0]);

  planet.render();

  // sun
  glm::mat4 modelMatrixSun{1.0f};
  modelMatrixSun = glm::translate(modelMatrixSun, m_sun.m_position);
  modelMatrixSun = glm::scale(modelMatrixSun, glm::vec3(16.0f));

  //  Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixSun[0][0]);

  sun.render();

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  if (control) {
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoNavInputs |
                                 ImGuiWindowFlags_NoDecoration};

    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x / 5.0f, (m_viewportSize.y) / 80.0f));
    ImGui::SetNextWindowSize(ImVec2(250, 170));
    ImGui::Begin("Widget window", nullptr, flags);
    ImGui::Text("tecla a = Mover para a esquerda");
    ImGui::Text("tecla d = Mover para a direita");
    ImGui::Text("tecla w = Mover para frente");
    ImGui::Text("tecla s = Mover para trás");
    ImGui::Text("tecla r = Mover para cima");
    ImGui::Text("tecla f = Mover para baixo");
    ImGui::Text("scroll wheel = Zoom");

    if (ImGui::Button("Ocultar Controles", ImVec2(0, 0))) {
      control = !control;
    }
    ImGui::End();
  }

  {
    auto widgetSize{ImVec2(150, 60)};
    ImGui::SetNextWindowPos(
        ImVec2(m_viewportSize.x / 4.0f, (m_viewportSize.y) / 80.0f));
    ImGui::SetNextWindowSize(widgetSize);

    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoNavInputs |
                                 ImGuiWindowFlags_NoDecoration};

    ImGui::Begin("Widget window", nullptr, flags);

    {

      if (ImGui::Button("Voltar ao Início", ImVec2(0, 0))) {
        eye = glm::vec3{0.0f, 0.0f, 0.0f};
      }
      if (ImGui::Button("Mostrar Controles", ImVec2(0, 0))) {
        control = !control;
      }
    }

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::checkCollision() {

  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  for (auto &star : m_stars) {
    auto const distance{glm::distance(eye, star.m_position)};

    if (distance < 1.0f + 0.2f * 0.85f) {

      eye -= 5.0f * deltaTime * at;
    }
  }

  auto const distanceSun{glm::distance(eye, m_sun.m_position)};
  if (distanceSun < 27.0f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distanceUfo{glm::distance(eye, m_ufo.m_position)};
  if (distanceUfo < 1.8f) {

    eye -= 5.0f * deltaTime * at;
  }

  auto const distancePlanet{glm::distance(eye, m_planet.m_position)};
  if (distancePlanet < 15.0f) {

    eye -= 5.0f * deltaTime * at;
  }
}

void Window::onDestroy() {
  ufo.destroy();
  abcg::glDeleteProgram(m_program);
}