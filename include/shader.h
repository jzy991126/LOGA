#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader {
public:
  // 程序ID
  unsigned int id_;

  // 构造器读取并构建着色器
  Shader(const char *vertex_path, const char *fragment_path,const char* geo_path= nullptr);
  // 使用/激活程序
  void Use();
  // uniform工具函数
  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, int value) const;
  void SetFloat(const std::string &name, float value) const;
  void SetMat4f(const std::string &name, const glm::mat4 &value) const;
  void SetVec3(const std::string &name, const glm::vec3 &value) const;
  void SetVec2(const std::string &name, const glm::vec2 &value) const;

public:
  static void CheckCompileErrors(unsigned int shader, std::string type);
};