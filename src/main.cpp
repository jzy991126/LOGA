#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

void renderCube();
void renderQuad();

int screen_width, screen_height;
float lastX = SCR_WIDTH / 2, lastY = SCR_HEIGHT / 2;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
bool first_mouse = true;
bool view_move = false;

unsigned int plane_vao;

void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);
unsigned int loadCubemap(vector<std::string> faces);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);

Camera camera(glm::vec3(0, 0, 5));

glm::vec3 light_pos(0.5f, 1.0f, 0.3f);

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  screen_width = 800;
  screen_height = 800;

  GLFWwindow *window = glfwCreateWindow(800, 800, "yang", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetCursorPosCallback(window, mouse_callback);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glViewport(0, 0, 800, 800);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_STENCIL_TEST);
  glDepthFunc(GL_LESS);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  float gird_plane[] = {-1, 0, -1, 1,  0, -1, 1,  0, 1,
                        1,  0, 1,  -1, 0, 1,  -1, 0, -1};

  unsigned int grid_plane_vao;
  glGenVertexArrays(1, &grid_plane_vao);
  glBindVertexArray(grid_plane_vao);
  unsigned int grid_plane_vbo;
  glGenBuffers(1, &grid_plane_vbo);

  glBindBuffer(GL_ARRAY_BUFFER, grid_plane_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(gird_plane), gird_plane, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // Model nanosuit("model/nanosuit/nanosuit.obj");
  Model planet("model/planet/planet.obj");
  Model rock("model/rock/rock.obj");
  //   Shader model_shader("shaders/3/model.vert", "shaders/3/model.frag");

  float quadVertices[] = {// positions   // texCoords
                          -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                          0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                          -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                          1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  float quadVertices1[] = {
      // positions   // texCoords
      0.5f, 1.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 1.0f, 0.0f,

      0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

  float skyboxVertices[] = {
      // positions
      -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
      -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

      1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

      -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

      -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
      1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

      -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
      1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

  float planeVertices[] = {
      // positions            // normals         // texcoords
      25.0f,  -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
      -25.0f, -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
      -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,

      25.0f,  -0.5f, 25.0f,  0.0f, 1.0f, 0.0f, 25.0f, 0.0f,
      -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 0.0f,  25.0f,
      25.0f,  -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f};

  std::vector<glm::vec3> lightPositions;
  lightPositions.emplace_back(0.0f, 0.5f, 1.5f);
  lightPositions.emplace_back(-4.0f, 0.5f, -3.0f);
  lightPositions.emplace_back(3.0f, 0.5f, 1.0f);
  lightPositions.emplace_back(-.8f, 2.4f, -1.0f);
  // colors
  std::vector<glm::vec3> lightColors;
  lightColors.emplace_back(5.0f, 5.0f, 5.0f);
  lightColors.emplace_back(10.0f, 0.0f, 0.0f);
  lightColors.emplace_back(0.0f, 0.0f, 15.0f);
  lightColors.emplace_back(0.0f, 5.0f, 0.0f);

  vector<std::string> faces{"right.jpg",  "left.jpg",  "top.jpg",
                            "bottom.jpg", "front.jpg", "back.jpg"};
  unsigned int wood_tex = loadTexture("img/wood.png");
  unsigned int brick_tex = loadTexture("img/brickwall.jpg");
  unsigned int norm_tex = loadTexture("img/brickwall_normal.jpg");
  unsigned int brick2_tex = loadTexture("img/bricks2.jpg");
  unsigned int brick2_norm_tex = loadTexture("img/bricks2_normal.jpg");
  unsigned int brick2_disp_tex = loadTexture("img/bricks2_disp.jpg");
  unsigned int container_tex = loadTexture("img/container2.png");

  unsigned int plane_vbo;
  glGenVertexArrays(1, &plane_vao);
  glGenBuffers(1, &plane_vbo);
  glBindVertexArray(plane_vao);
  glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int quadVAO1, quadVBO1;
  glGenVertexArrays(1, &quadVAO1);
  glGenBuffers(1, &quadVBO1);
  glBindVertexArray(quadVAO1);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO1);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices1), quadVertices1,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int skybox_vao, skybox_vbo;
  glGenVertexArrays(1, &skybox_vao);
  glGenBuffers(1, &skybox_vbo);
  glBindVertexArray(skybox_vao);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glBindVertexArray(0);

  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  GLuint colorBuffers[2];
  glGenTextures(2, colorBuffers);
  for (GLuint i = 0; i < 2; i++) {
    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, colorBuffers[i], 0);
  }
  GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 800);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  unsigned int depth_map_fbo;
  glGenFramebuffers(1, &depth_map_fbo);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
  unsigned int depth_map;
  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH,
               SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depth_map, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
              << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  unsigned int pingpongFBO[2];
  unsigned int pingpongColorbuffers[2];
  glGenFramebuffers(2, pingpongFBO);
  glGenTextures(2, pingpongColorbuffers);
  for (unsigned int i = 0; i < 2; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0,
                 GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would
                           // otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pingpongColorbuffers[i], 0);
    // also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
  }

  unsigned int cubemapTexture = loadCubemap(faces);

  Shader fbo_shader("shaders/6/fbo.vert", "shaders/6/fbo.frag");
  Shader skybox_shader("shaders/7/skymap.vert", "shaders/7/skymap.frag");
  Shader floor_shader("shaders/9/bp.vert", "shaders/9/bp.frag");
  Shader grid_plane_shader("shaders/2/plane.vert", "shaders/2/plane.frag");
  Shader depth_shader("shaders/10/depth.vert", "shaders/10/depth.frag");
  Shader debug_depth_shader("shaders/6/fbo.vert", "shaders/6/depth.frag");
  Shader normal_map_shader("shaders/11/normalmap.vert",
                           "shaders/11/normalmap.frag");
  Shader blur_shader("shaders/13/blur.vert", "shaders/13/blur.frag");
  // Shader
  // lighting_shader("shaders/12/lighting.vert","shaders/12/lighting.frag");

  Shader bloom_shader("shaders/13/bloom.vert", "shaders/13/bloom.frag");
  Shader bloom_blend_shader("shaders/13/bloom_blend.vert",
                            "shaders/13/bloom_blend.frag");
  Shader light_cube_shader("shaders/13/bloom.vert",
                           "shaders/13/lightcube.frag");

  bloom_shader.Use();
  bloom_shader.SetInt("diff_tex", 0);
  for (unsigned int i = 0; i < lightPositions.size(); i++) {
    bloom_shader.SetVec3("lights[" + std::to_string(i) + "].position",
                         lightPositions[i]);
    bloom_shader.SetVec3("lights[" + std::to_string(i) + "].color",
                         lightColors[i]);
  }

  floor_shader.Use();
  floor_shader.SetInt("floorTexture", 0);
  floor_shader.SetInt("shadowMap", 1);

  bloom_blend_shader.Use();
  bloom_blend_shader.SetInt("scene", 0);
  bloom_blend_shader.SetInt("bloomBlur", 1);

  blur_shader.Use();
  blur_shader.SetInt("image", 0);

  glm::mat4 light_projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 1.0f, 7.5f);
  glm::mat4 light_view = glm::lookAt(light_pos, glm::vec3(0.f, 0.f, 0.f),
                                     glm::vec3(0.f, 1.f, 0.f));
  auto light_space_mat = light_projection * light_view;

  depth_shader.Use();
  depth_shader.SetMat4f("lightSpaceMatrix", light_space_mat);
  depth_shader.SetMat4f("model", glm::mat4(1.0));

  normal_map_shader.Use();
  normal_map_shader.SetInt("diff_tex", 0);
  normal_map_shader.SetInt("norm_tex", 1);
  normal_map_shader.SetInt("disp_tex", 2);
  normal_map_shader.SetVec3("light_pos", light_pos);

  bool b_use_bloom = false;
  float exposure = 0.1f;

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    process_input(window);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    glfwSetScrollCallback(window, scroll_callback);

    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         float(screen_width) / screen_height, 0.01f, 10000.0f);

    {

      //      glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
      //      glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
      //      glEnable(GL_DEPTH_TEST);
      //      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      //      glClear(GL_COLOR_BUFFER_BIT |
      //              GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲
      //      depth_shader.Use();
      //      glActiveTexture(GL_TEXTURE0);
      //      glBindTexture(GL_TEXTURE_2D, floor_tex);
      //      renderScene(depth_shader);
      //      glBindVertexArray(0);
    }
    glViewport(0, 0, SCR_WIDTH, SCR_WIDTH);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲
    glEnable(GL_DEPTH_TEST);

    {

      bloom_shader.Use();
      bloom_shader.SetMat4f("projection", projection);
      bloom_shader.SetMat4f("view", camera.GetViewMatrix());
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, wood_tex);

      auto model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
      model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      // ----------------
      //  container

      glBindTexture(GL_TEXTURE_2D, container_tex);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
      model = glm::scale(model, glm::vec3(0.5f));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
      model = glm::scale(model, glm::vec3(0.5f));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
      model = glm::rotate(model, glm::radians(60.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
      model = glm::rotate(model, glm::radians(23.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      model = glm::scale(model, glm::vec3(1.25));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
      model = glm::rotate(model, glm::radians(124.0f),
                          glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
      model = glm::scale(model, glm::vec3(0.5f));
      bloom_shader.SetMat4f("model", model);
      renderCube();

      // ---------------
      // light cube

      light_cube_shader.Use();
      light_cube_shader.SetMat4f("projection", projection);
      light_cube_shader.SetMat4f("view", camera.GetViewMatrix());

      for (unsigned int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPositions[i]));
        model = glm::scale(model, glm::vec3(0.25f));
        light_cube_shader.SetMat4f("model", model);
        light_cube_shader.SetVec3("lightColor", lightColors[i]);
        renderCube();
      }

      // ---------
      //------blur
      bool horizontal = true, first_iteration = true;
      unsigned int amount = 10;
      blur_shader.Use();
      for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        blur_shader.SetInt("horizontal", horizontal);
        glBindTexture(
            GL_TEXTURE_2D,
            first_iteration
                ? colorBuffers[1]
                : pingpongColorbuffers[!horizontal]); // bind texture of other
                                                      // framebuffer (or scene
                                                      // if first iteration)
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
          first_iteration = false;
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    {

        //      glm::mat4 model = glm::mat4(1.0f);
        //      model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
        //      model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
        //
        //      lighting_shader.Use();
        //      lighting_shader.SetMat4f("projection", projection);
        //      lighting_shader.SetMat4f("view", camera.GetViewMatrix());
        //      lighting_shader.SetMat4f("model", model);
        //
        //      lighting_shader.SetInt("diff_tex", 0);
        //      for (unsigned int i = 0; i < lightPositions.size(); i++) {
        //        lighting_shader.SetVec3("lights[" + std::to_string(i) +
        //        "].position",
        //                                lightPositions[i]);
        //        lighting_shader.SetVec3("lights[" + std::to_string(i) +
        //        "].color",
        //                                lightColors[i]);
        //      }
        //
        //      glActiveTexture(GL_TEXTURE0);
        //      glBindTexture(GL_TEXTURE_2D, floor_tex);
        //      renderCube();
    } {

        //      floor_shader.Use();
        //      floor_shader.SetMat4f("projection", projection);
        //      floor_shader.SetMat4f("view", camera.GetViewMatrix());
        //      // set light uniforms
        //      floor_shader.SetVec3("viewPos", camera.Position);
        //      floor_shader.SetVec3("lightPos", light_pos);
        //      floor_shader.SetInt("blinn", b_use_bloom);
        //      floor_shader.SetMat4f("lightSpaceMatrix", light_space_mat);
        //
        //      //      glBindVertexArray(plane_vao);
        //      glActiveTexture(GL_TEXTURE0);
        //      glBindTexture(GL_TEXTURE_2D, floor_tex);
        //      glActiveTexture(GL_TEXTURE1);
        //      glBindTexture(GL_TEXTURE_2D, depth_map);
        //      // glDrawArrays(GL_TRIANGLES, 0, 6);
        //      renderScene(floor_shader);
        //      glBindVertexArray(0);
    }

    {

        //      glm::mat4 model(1.0);
        //      //      model = glm::rotate(model, (GLfloat)glfwGetTime() * -2,
        //      //                          glm::normalize(glm::vec3(1.0,
        //      0.0, 1.0)));
        //      normal_map_shader.Use();
        //      normal_map_shader.SetMat4f("projection", projection);
        //      normal_map_shader.SetMat4f("view", camera.GetViewMatrix());
        //      normal_map_shader.SetMat4f("model", model);
        //      normal_map_shader.SetVec3("view_pos", camera.Position);
        //      normal_map_shader.SetInt("norm_map", b_use_bloom);
        //      normal_map_shader.SetFloat("exposure", exposure);
        //      glActiveTexture(GL_TEXTURE0);
        //      glBindTexture(GL_TEXTURE_2D, brick2_tex);
        //      glActiveTexture(GL_TEXTURE1);
        //      glBindTexture(GL_TEXTURE_2D, brick2_norm_tex);
        //      glActiveTexture(GL_TEXTURE2);
        //      glBindTexture(GL_TEXTURE_2D, brick2_disp_tex);
        //      renderQuad();
    }

    {
        //      glDepthFunc(GL_LEQUAL);
        //      glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        //      skybox_shader.Use();
        //      skybox_shader.SetMat4f("view", view);
        //      skybox_shader.SetMat4f("projection", projection);
        //      glBindVertexArray(skybox_vao);
        //      glActiveTexture(GL_TEXTURE0);
        //      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        //      glDrawArrays(GL_TRIANGLES, 0, 36);
        //      glBindVertexArray(0);
        //      glDepthFunc(GL_LESS);
    }

    {

        //      glBindVertexArray(grid_plane_vao);
        //      grid_plane_shader.Use();
        //      grid_plane_shader.SetMat4f("view", camera.GetViewMatrix());
        //      grid_plane_shader.SetMat4f("projection", projection);
        //      grid_plane_shader.SetMat4f("inv_view",
        //                                 glm::inverse(camera.GetViewMatrix()));
        //      grid_plane_shader.SetMat4f("inv_proj",
        //      glm::inverse(projection));
        //      glDrawArrays(GL_TRIANGLES, 0, 6);
        //      glBindVertexArray(0);
    }

    {
      glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      //      fbo_shader.Use();
      //      fbo_shader.SetFloat("exposure", exposure);
      //      glBindVertexArray(quadVAO);
      //      glDisable(GL_DEPTH_TEST);
      //      glActiveTexture(GL_TEXTURE0);
      //      glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[0]);
      //      glDrawArrays(GL_TRIANGLES, 0, 6);
      //
      //      debug_depth_shader.Use();
      //      glBindVertexArray(quadVAO1);
      //      glActiveTexture(GL_TEXTURE0);
      //      glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1]);
      //      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    {

      bloom_blend_shader.Use();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[1]);
      bloom_blend_shader.SetInt("bloom", b_use_bloom);
      bloom_blend_shader.SetFloat("exposure", exposure);
      renderQuad();
    }

    ImGui::Begin("Demo window");
    ImGui::Checkbox("bloom", &b_use_bloom);
    ImGui::DragFloat("exposure", &exposure, 0.1f, 0.f, 10.f);
    ImGui::Text("%.2f FPS", 1.f / deltaTime);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwTerminate();
  return 0;
}

void frame_buffer_size_callback(GLFWwindow *window, int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    camera.ProcessKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    camera.ProcessKeyboard(DOWN, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    camera.MouseSensitivity = 0.2;
    camera.MovementSpeed = 5.0;
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
    camera.MouseSensitivity = 0.1;
    camera.MovementSpeed = 2.5;
  }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  if (!view_move)
    return;

  auto xpos = static_cast<float>(xposIn);
  auto ypos = static_cast<float>(yposIn);

  if (first_mouse) {
    lastX = xpos;
    lastY = ypos;
    first_mouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const *path) {
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(
        GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
        format == GL_RGBA
            ? GL_CLAMP_TO_EDGE
            : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent
                          // semi-transparent borders. Due to interpolation it
                          // takes texels from next repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

unsigned int loadCubemap(vector<std::string> faces) {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
  string img_path("img/skybox/");

  int width, height, nrChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char *data = stbi_load((img_path + faces[i]).c_str(), &width,
                                    &height, &nrChannels, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, data);
      stbi_image_free(data);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << faces[i]
                << std::endl;
      stbi_image_free(data);
    }
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return textureID;
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    first_mouse = true;
    view_move = true;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    view_move = false;
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void renderCube() {
  // initialize (if necessary)
  if (cubeVAO == 0) {
    float vertices[] = {
        // back face
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,  // top-left
        // front face
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // top-right
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
        -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
        -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,   // top-right
                                                            // right face
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,    // top-right
        1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // bottom-right
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,     // top-left
        1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,    // bottom-left
        // bottom face
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,  // top-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,   // bottom-left
        -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
        -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,  // top-right
        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // bottom-right
        -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
        -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f   // bottom-left
    };
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    // fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // link vertex attributes
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
  // render Cube
  glBindVertexArray(cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
  if (quadVAO == 0) {
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}
