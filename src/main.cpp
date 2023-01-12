#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "shader.h"
#include "model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <random>
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

int screen_width, screen_height;
float lastX = float(SCR_WIDTH) / 2, lastY = float(SCR_HEIGHT) / 2;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
bool first_mouse = true;
bool view_move = false;

void frame_buffer_size_callback(GLFWwindow *window, int width, int height);
void process_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
unsigned int loadTexture(char const *path);
void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods);
void renderSphere();
GLFWwindow *InitWindow();

Camera camera(glm::vec3(0, 0, 5));
glm::vec3 light_pos(0.5f, 1.0f, 0.3f);

int main() {

  auto window = InitWindow();

  Shader pbr_shader("shaders/16/pbr.vert", "shaders/16/pbr.frag");

  unsigned int albedo_tex = loadTexture("img/rusted/basecolor.png");
  unsigned int metallic_tex = loadTexture("img/rusted/metallic.png");
  unsigned int normal_tex = loadTexture("img/rusted/normal.png");
  unsigned int roughness_tex = loadTexture("img/rusted/roughness.png");

  pbr_shader.Use();
  pbr_shader.SetInt("albedoMap", 0);
  pbr_shader.SetInt("normalMap", 1);
  pbr_shader.SetInt("metallicMap", 2);
  pbr_shader.SetInt("roughnessMap", 3);

  glm::vec3 albedo(1.0);
  float ao = 1.0;

  glm::vec3 lightPositions[] = {
      glm::vec3(-10.0f, 10.0f, 10.0f),
      glm::vec3(10.0f, 10.0f, 10.0f),
      glm::vec3(-10.0f, -10.0f, 10.0f),
      glm::vec3(10.0f, -10.0f, 10.0f),
  };
  glm::vec3 lightColors[] = {
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
      glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f)};

  int nrRows = 7;
  int nrColumns = 7;
  float spacing = 2.5;

  while (!glfwWindowShouldClose(window)) {
    {
      glfwPollEvents();
      process_input(window);
      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();
    }

    {
      glViewport(0, 0, SCR_WIDTH, SCR_WIDTH);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    {

      pbr_shader.Use();
      glm::mat4 view = camera.GetViewMatrix();
      pbr_shader.SetMat4f("view", view);
      pbr_shader.SetVec3("camPos", camera.Position);
      pbr_shader.SetMat4f("projection", projection);
      pbr_shader.SetVec3("albedo", albedo);
      pbr_shader.SetFloat("ao", ao);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, albedo_tex);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, normal_tex);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, metallic_tex);
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, roughness_tex);

      // render rows*column number of spheres with varying metallic/roughness
      // values scaled by rows and columns respectively
      glm::mat4 model = glm::mat4(1.0f);
      for (int row = 0; row < nrRows; ++row) {
        pbr_shader.SetFloat("metallic", (float)row / (float)nrRows);
        for (int col = 0; col < nrColumns; ++col) {
          // we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces
          // (roughness of 0.0) tend to look a bit off on direct lighting.
          pbr_shader.SetFloat(
              "roughness",
              glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

          model = glm::mat4(1.0f);
          model = glm::translate(
              model, glm::vec3((col - (nrColumns / 2)) * spacing,
                               (row - (nrRows / 2)) * spacing, 0.0f));
          pbr_shader.SetMat4f("model", model);
          renderSphere();
        }
      }

      // render light source (simply re-render sphere at light positions)
      // this looks a bit off as we Use the same pbr_shader, but it'll make
      // their positions obvious and keeps the codeprint small.
      for (unsigned int i = 0;
           i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i) {
        glm::vec3 newPos = lightPositions[i] +
                           glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
        newPos = lightPositions[i];
        pbr_shader.SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
        pbr_shader.SetVec3("lightColors[" + std::to_string(i) + "]",
                           lightColors[i]);

        model = glm::mat4(1.0f);
        model = glm::translate(model, newPos);
        model = glm::scale(model, glm::vec3(0.5f));
        pbr_shader.SetMat4f("model", model);
        renderSphere();
      }
    }

    {
      ImGui::Begin("info window");
      ImGui::Text("%.2f FPS", 1.f / deltaTime);
      ImGui::SliderFloat("ao", &ao, 0.f, 1.f);
      ImGui::ColorEdit3("albedo", glm::value_ptr(albedo));
      ImGui::End();
    }

    auto currentFrame = float(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    {
      ImGui::Render();
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
      glfwSwapBuffers(window);
    }
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
            : GL_REPEAT); // for this tutorial: Use GL_CLAMP_TO_EDGE to prevent
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

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    first_mouse = true;
    view_move = true;
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
    view_move = false;
}
GLFWwindow *InitWindow() {
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
  glfwSetScrollCallback(window, scroll_callback);
  glEnable(GL_DEPTH_TEST);
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init();
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  return window;
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere() {
  if (sphereVAO == 0) {
    glGenVertexArrays(1, &sphereVAO);

    unsigned int vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
      if (!oddRow) // even rows: y == 0, y == 2; and so on
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      } else {
        for (int x = X_SEGMENTS; x >= 0; --x) {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)(6 * sizeof(float)));
  }

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}