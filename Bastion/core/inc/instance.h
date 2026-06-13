//
// Created by sjadoona on 2026-05-27.
//

#ifndef BASTION_INSTANCE_H
#define BASTION_INSTANCE_H

#include <vulkan/vulkan_raii.hpp>
#include "app_info.h"
#include "raii_glfw_window.h"

namespace Bastion
{
  class Instance
  {
  private:
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;
    bool hasDebugUtils = false;

  public:
    [[nodiscard]] vk::raii::Instance& getInstance();
    [[nodiscard]] vk::raii::SurfaceKHR& getSurface();

    void createInstance();
    void setupDebugMessenger();
    void createSurface(raiiGLFWwindow& window);

  private:
    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
      vk::DebugUtilsMessageTypeFlagsEXT type,
      const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);
  };
} // Bastion

#endif //BASTION_INSTANCE_H
