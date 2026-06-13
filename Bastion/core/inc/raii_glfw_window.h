//
// Created by sjadoona on 2026-06-06.
//

#ifndef BASTION_RAII_GLFW_WINDOW_H
#define BASTION_RAII_GLFW_WINDOW_H

#include <GLFW/glfw3.h>
#include <memory>

struct DestroyGLFWwindow
{
  void operator()(GLFWwindow* ptr)
  {
    glfwDestroyWindow(ptr);
  }
};

using raiiGLFWwindow = std::unique_ptr<GLFWwindow, DestroyGLFWwindow>;

#endif //BASTION_RAII_GLFW_WINDOW_H
