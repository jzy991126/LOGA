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
bool view_move = false;

void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);
unsigned int loadCubemap(vector<std::string> faces);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);

Camera camera(glm::vec3(0, 0, 3));

glm::vec3 light_pos(0, 0, 0);

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
  Shader grid_plane_shader("shaders/2/plane.vert", "shaders/2/plane.frag");

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
      10.0f,  -0.5f, 10.0f,  0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
      -10.0f, -0.5f, 10.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f,
      -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f,  10.0f,

      10.0f,  -0.5f, 10.0f,  0.0f, 1.0f, 0.0f, 10.0f, 0.0f,
      -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f,  10.0f,
      10.0f,  -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f};

  vector<std::string> faces{"right.jpg",  "left.jpg",  "top.jpg",
                            "bottom.jpg", "front.jpg", "back.jpg"};
  unsigned int floor_tex = loadTexture("img/wood.png");

  unsigned int plane_vao, plane_vbo;
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

  unsigned int cubemapTexture = loadCubemap(faces);

  Shader fbo_shader("shaders/6/fbo.vert", "shaders/6/fbo.frag");
  Shader skybox_shader("shaders/7/skymap.vert", "shaders/7/skymap.frag");
  Shader floor_shader("shaders/9/bp.vert", "shaders/9/bp.frag");

  floor_shader.Use();
  floor_shader.SetInt("floorTexture", 0);

  bool use_blinn = false;

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
                         float(screen_width) / screen_height, 1.f, 10000.0f);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // 我们现在不使用模板缓冲

    {
      floor_shader.Use();
      floor_shader.SetMat4f("projection", projection);
      floor_shader.SetMat4f("view", camera.GetViewMatrix());
      // set light uniforms
      floor_shader.SetVec3("viewPos", camera.Position);
      floor_shader.SetVec3("lightPos", light_pos);
      floor_shader.SetInt("blinn", use_blinn);

      glBindVertexArray(plane_vao);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, floor_tex);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      glBindVertexArray(0);
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
//      grid_plane_shader.SetMat4f("inv_proj", glm::inverse(projection));
//      glDrawArrays(GL_TRIANGLES, 0, 6);
//      glBindVertexArray(0);
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
    auto res = ImGui::Button("Hello!");
    if (res)
      cout << "click" << endl;
    // ImGui::Bullet();
    ImGui::Checkbox("bl", &use_blinn);
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