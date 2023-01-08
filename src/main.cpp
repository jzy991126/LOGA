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

int screen_width, screen_height;
float lastX = 400, lastY = 400;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
bool first_mouse = true;

void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);

Camera camera(glm::vec3(0, 1, 3));

glm::vec3 light_pos(1.2f, 1.f, 2.f);

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  screen_width = 800;
  screen_height = 800;

  GLFWwindow *window = glfwCreateWindow(800, 800, "yang", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  glViewport(0, 0, 800, 800);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
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
  Shader plane_shader("shaders/2/plane.vert", "shaders/2/plane.frag");

  float cubeVertices[] = {
      // positions          // texture Coords
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
  float planeVertices[] = {
      // positions          // texture Coords (note we set these higher than 1
      // (together with GL_REPEAT as texture wrapping mode). this will cause the
      // floor texture to repeat)
      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, 5.0f,
      0.0f, 0.0f,  -5.0f, -0.5f, -5.0f, 0.0f,  2.0f,

      5.0f, -0.5f, 5.0f,  2.0f,  0.0f,  -5.0f, -0.5f, -5.0f,
      0.0f, 2.0f,  5.0f,  -0.5f, -5.0f, 2.0f,  2.0f};

  //  Model nanosuit("model/nanosuit/nanosuit.obj");
  //  Shader model_shader("shaders/3/model.vert", "shaders/3/model.frag");

  float transparentVertices[] = {
      // positions         // texture Coords (swapped y coordinates because
      // texture is flipped upside down)
      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -0.5f, 0.0f,
      0.0f, 1.0f, 1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

      0.0f, 0.5f, 0.0f, 0.0f,  0.0f, 1.0f, -0.5f, 0.0f,
      1.0f, 1.0f, 1.0f, 0.5f,  0.0f, 1.0f, 0.0f};

  vector<glm::vec3> windows{
      glm::vec3(-1.5f, 0.0f, -0.48f), glm::vec3(1.5f, 0.0f, 0.51f),
      glm::vec3(0.0f, 0.0f, 0.7f), glm::vec3(-0.3f, 0.0f, -2.3f),
      glm::vec3(0.5f, 0.0f, -0.6f)};

  float quadVertices[] = {// positions   // texCoords
                          -1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                          0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,

                          -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                          1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  float quadVertices1[] = {// positions   // texCoords
                          0.5f, 1.0f, 0.0f, 1.0f,  0.5f, 0.5f,
                          0.0f,  0.0f, 1.0f, 0.5f, 1.0f,  0.0f,

                          0.5f, 1.0f, 0.0f, 1.0f,  1.0f,  0.5f,
                          1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};

  std::map<float, glm::vec3> sorted;
  for (unsigned int i = 0; i < windows.size(); i++) {
    float distance = glm::length(camera.Position - windows[i]);
    sorted[distance] = windows[i];
  }
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  unsigned int cubeVAO, cubeVBO;
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);
  // plane VAO
  unsigned int planeVAO, planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glBindVertexArray(0);

  unsigned int transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices),
               transparentVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
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


  unsigned int fbo;
  glGenFramebuffers(1, &fbo);
  unsigned int fbo_texture;
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glGenTextures(1, &fbo_texture);
  glBindTexture(GL_TEXTURE_2D, fbo_texture);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 800, 0, GL_RGB, GL_UNSIGNED_BYTE,
               NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         fbo_texture, 0);

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

  vector<glm::vec3> vegetation;
  vegetation.emplace_back(-1.5f, 0.0f, -0.48f);
  vegetation.emplace_back(1.5f, 0.0f, 0.51f);
  vegetation.emplace_back(0.0f, 0.0f, 0.7f);
  vegetation.emplace_back(-0.3f, 0.0f, -2.3f);
  vegetation.emplace_back(0.5f, 0.0f, -0.6f);

  unsigned int cubeTexture = loadTexture("img/marble.jpg");
  unsigned int floorTexture = loadTexture("img/metal.png");
  unsigned int transparentTexture =
      loadTexture("img/blending_transparent_window.png");
  Shader cube_shader("shaders/4/cube.vert", "shaders/4/cube.frag");
  Shader single_shader("shaders/4/cube.vert", "shaders/4/single.frag");
  Shader fbo_shader("shaders/6/fbo.vert", "shaders/6/fbo.frag");

  // glStencilMask(0x00); // 每一位写入模板缓冲时都保持原样
  //  glStencilFunc(GL_EQUAL, 1, 0xFF);
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

    // glm::mat4 model(1.0);
    // model = glm::rotate(model, float(glfwGetTime()), glm::vec3(1., 0., 0.));

    glm::mat4 projection;
    projection =
        glm::perspective(glm::radians(camera.Zoom),
                         float(screen_width) / screen_height, 0.1f, 100.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲

    //    glClearColor(0.2, 0.2, 0.2, 1.0);
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
    //    GL_STENCIL_BUFFER_BIT);

    {
      glEnable(GL_DEPTH_TEST);
      //      glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
      //      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
      //      GL_STENCIL_BUFFER_BIT);

      // glStencilMask(0x00); // 记得保证我们在绘制地板的时候不会更新模板缓冲
      glBindVertexArray(planeVAO);
      glm::mat4 model(1.0);
      cube_shader.Use();
      cube_shader.SetMat4f("projection", projection);
      cube_shader.SetMat4f("view", camera.GetViewMatrix());
      cube_shader.SetMat4f("model", model);
      glBindTexture(GL_TEXTURE_2D, floorTexture);

      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
    }

    {

      //      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      //      glStencilMask(0xFF);
      auto model = glm::mat4(1.0f);
      glBindVertexArray(cubeVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, cubeTexture);
      model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
      cube_shader.SetMat4f("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
      cube_shader.SetMat4f("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      // floor
    }

    {
      glBindVertexArray(transparentVAO);
      glBindTexture(GL_TEXTURE_2D, transparentTexture);
      for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        auto model = glm::mat4(1.0f);
        model = glm::translate(model, it->second);
        cube_shader.SetMat4f("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
      }
    }

    {
        //      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        //      glStencilMask(0x00);
        //      //glDisable(GL_DEPTH_TEST);
        //
        //      glBindVertexArray(cubeVAO);
        //      glActiveTexture(GL_TEXTURE0);
        //      glBindTexture(GL_TEXTURE_2D, cubeTexture);
        //      float scale = 1.1f;
        //
        //      single_shader.Use();
        //      single_shader.SetMat4f("projection", projection);
        //      single_shader.SetMat4f("view", camera.GetViewMatrix());
        //
        //      auto model = glm::mat4(1.0f);
        //      model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        //      model = glm::scale(model, glm::vec3(scale, scale, scale));
        //      single_shader.SetMat4f("model", model);
        //      glDrawArrays(GL_TRIANGLES, 0, 36);
        //
        //      model = glm::mat4(1.0f);
        //      model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        //      model = glm::scale(model, glm::vec3(scale, scale, scale));
        //      single_shader.SetMat4f("model", model);
        //      glDrawArrays(GL_TRIANGLES, 0, 36);
        //      glBindVertexArray(0);
        //      glStencilMask(0xFF);
        //      glStencilFunc(GL_ALWAYS, 0, 0xFF);
        //      glEnable(GL_DEPTH_TEST);
    }

    {
        //      model_shader.Use();
        //      model_shader.SetMat4f("projection", projection);
        //      model_shader.SetMat4f("view", camera.GetViewMatrix());
        //      model_shader.SetMat4f("model", model);
        //      nanosuit.Draw(model_shader);
    }

    {
      // glDisable(GL_DEPTH_TEST);
      glBindVertexArray(grid_plane_vao);
      plane_shader.Use();
      plane_shader.SetMat4f("view", camera.GetViewMatrix());
      plane_shader.SetMat4f("projection", projection);
      plane_shader.SetMat4f("inv_view", glm::inverse(camera.GetViewMatrix()));
      plane_shader.SetMat4f("inv_proj", glm::inverse(projection));
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
      // glEnable(GL_DEPTH_TEST);
    }

    {
      glBindFramebuffer(GL_FRAMEBUFFER, 0); // 返回默认
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      fbo_shader.Use();
      glBindVertexArray(quadVAO);
      glDisable(GL_DEPTH_TEST);
      glBindTexture(GL_TEXTURE_2D, fbo_texture);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glBindVertexArray(quadVAO1);
      glDrawArrays(GL_TRIANGLES, 0, 6);


    }

    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
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
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {

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