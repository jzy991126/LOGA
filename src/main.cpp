#include <glad/glad.h>
#include <GLFW/glfw3.h>

void frame_buffer_size_callback(GLFWwindow* window,int width,int height);
void process_input(GLFWwindow * window);
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
  //glfwWindowHint(GLFW_DECORATED, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(800, 800, "yang",nullptr, nullptr);
  glfwMakeContextCurrent(window);

  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  
  glViewport(0,0,800,800);
  glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

  while(!glfwWindowShouldClose(window))
  {
    process_input(window);

    glClearColor(0.2,0.2,0.2,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

void frame_buffer_size_callback(GLFWwindow* window,int width,int height)
{
    glViewport(0,0,width,height);
}

void process_input(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}