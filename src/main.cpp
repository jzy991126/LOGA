#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "camera.h"
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
  float vertices[] = {
      // positions          // normals           // texture coords
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f,  0.0f,  0.5f,  0.5f,  -0.5f, 0.0f,
      0.0f,  -1.0f, 1.0f,  1.0f,  0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.5f,  -0.5f,
      0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      0.0f,  1.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
      1.0f,  1.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,
      -0.5f, -1.0f, 0.0f,  0.0f,  1.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
      0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.0f,

      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,
      -0.5f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,
      0.0f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
      0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,  0.5f,  -0.5f,
      -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f,  1.0f,  0.5f,  -0.5f, 0.5f,  0.0f,
      -1.0f, 0.0f,  1.0f,  0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
      1.0f,  0.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f,  0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f,  1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.5f,  0.5f,
      -0.5f, 0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,
      1.0f,  0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
      1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f,  1.0f};

  float gird_plane[] = {-1,  0,  -1, 1, 0, -1, 1, 0,  1,
                        1, 0, 1, -1,  0,  1, -1,  0, -1};

  glm::vec3 cubePositions[] = {
      glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
      glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
      glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f)};

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

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // unsigned int EBO;
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
  //              GL_STATIC_DRAW);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glBindVertexArray(0);

  unsigned int diff_tex = loadTexture("img/container2.png");
  unsigned int spec_tex = loadTexture("img/container2_specular.png");
  unsigned int emis_tex = loadTexture("img/matrix.jpg");

  unsigned int light_vao;
  glGenVertexArrays(1, &light_vao);
  glBindVertexArray(light_vao);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  Shader obj_shader("shaders/2/shader.vert", "shaders/2/shader.frag");
  Shader light_shader("shaders/2/shader.vert", "shaders/2/light.frag");
  Shader plane_shader("shaders/2/plane.vert", "shaders/2/plane.frag");

  glm::vec3 obj_color(1, 0.5, 0.31f);
  glm::vec3 light_color(1.f, 1.f, 1.f);
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

    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // shader.SetFloat("ti", green);
    //    glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
    //    glBindTexture(GL_TEXTURE_2D, texture);
    //    glActiveTexture(GL_TEXTURE1);
    //    glBindTexture(GL_TEXTURE_2D, texture1);

    //    float light_pos_z = 10 * sin(glfwGetTime());
    //    float light_pos_x = 10 * cos(glfwGetTime());
    //    light_pos = glm::vec3(light_pos_x, 1.f, light_pos_z);

    {
      glBindVertexArray(VAO); //
      obj_shader.Use();
      obj_shader.SetMat4f("view", camera.GetViewMatrix());
      obj_shader.SetMat4f("projection", projection);
      obj_shader.SetVec3("objectColor", obj_color);
      obj_shader.SetVec3("lightColor", light_color);
      obj_shader.SetVec3("view_pos", camera.Position);
      obj_shader.SetVec3("material.ambient",
                         glm::vec3(0.19225, 0.19225, 0.19225));
      obj_shader.SetVec3("material.specular",
                         glm::vec3(0.508273, 0.508273, 0.508273));
      obj_shader.SetFloat("material.shininess", 0.4 * 128);
      obj_shader.SetVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
      obj_shader.SetVec3(
          "light.diffuse",
          glm::vec3(0.5f, 0.5f, 0.5f)); // 将光照调暗了一些以搭配场景
      obj_shader.SetVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
      obj_shader.SetVec3("light.position", light_pos);
      obj_shader.SetInt("material.diffuse", 0);
      obj_shader.SetInt("material.specular", 1);
      obj_shader.SetInt("material.emission", 2);
      obj_shader.SetFloat("emission_strength", sin(glfwGetTime()) + 1.f);
      obj_shader.SetFloat("matrix_move", glfwGetTime());
      obj_shader.SetFloat("light.constant", 1.0f);
      obj_shader.SetFloat("light.linear", 0.09f);
      obj_shader.SetFloat("light.quadratic", 0.032f);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, diff_tex);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, spec_tex);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, emis_tex);

      for (unsigned int i = 0; i < 10; i++) {
        glm::mat4 model(1.f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle),
                            glm::vec3(1.0f, 0.3f, 0.5f));
        obj_shader.SetMat4f("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
      }

      //      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
    }

    {
      glBindVertexArray(light_vao);
      light_shader.Use();
      light_shader.SetMat4f("view", camera.GetViewMatrix());
      light_shader.SetMat4f("projection", projection);



      auto model = glm::mat4(1.f);
      model = glm::translate(model, light_pos);
      model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
      light_shader.SetMat4f("model", model);
      model = glm::scale(model, glm::vec3(0.2f));
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
    }

    {
      glBindVertexArray(grid_plane_vao);
      plane_shader.Use();
      plane_shader.SetMat4f("view", camera.GetViewMatrix());
      plane_shader.SetMat4f("projection", projection);
      plane_shader.SetMat4f("inv_view", glm::inverse(camera.GetViewMatrix()));
      plane_shader.SetMat4f("inv_proj", glm::inverse(projection));
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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