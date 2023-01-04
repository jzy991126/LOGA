#include "shader.h"

Shader::Shader(const char *vertex_path, const char *fragment_path) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vertex_code;
  std::string fragment_code;
  std::ifstream vertex_shader_file;
  std::ifstream fragment_shader_file;
  // ensure ifstream objects can throw exceptions:
  vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragment_shader_file.exceptions(std::ifstream::failbit |
                                  std::ifstream::badbit);
  try {
    // open files
    vertex_shader_file.open(vertex_path);
    fragment_shader_file.open(fragment_path);
    std::stringstream vertex_shader_stream, fragment_shader_stream;
    // read file's buffer contents into streams
    vertex_shader_stream << vertex_shader_file.rdbuf();
    fragment_shader_stream << fragment_shader_file.rdbuf();
    // close file handlers
    vertex_shader_file.close();
    fragment_shader_file.close();
    // convert stream into string
    vertex_code = vertex_shader_stream.str();
    fragment_code = fragment_shader_stream.str();
  } catch (std::ifstream::failure &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what()
              << std::endl;
  }
  const char *vertex_shader_code = vertex_code.c_str();
  const char *fragment_shader_code = fragment_code.c_str();
  // 2. compile shaders
  unsigned int vertex, fragment;
  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertex_shader_code, NULL);
  glCompileShader(vertex);
  CheckCompileErrors(vertex, "VERTEX");
  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragment_shader_code, NULL);
  glCompileShader(fragment);
  CheckCompileErrors(fragment, "FRAGMENT");
  // shader Program
  id_ = glCreateProgram();
  glAttachShader(id_, vertex);
  glAttachShader(id_, fragment);
  glLinkProgram(id_);
  CheckCompileErrors(id_, "PROGRAM");
  // delete the shaders as they're linked into our program now and no longer
  // necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::Use() { glUseProgram(id_); }
void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::SetFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name.c_str()), value);
}
void Shader::SetMat4f(const std::string &name, GLfloat *value) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE,
                     value);
}
void Shader::CheckCompileErrors(unsigned int shader, std::string type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cout
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
}