//
// Created by sjadoona on 2026-06-06.
//

#ifndef BASTION_SWAPCHAIN_H
#define BASTION_SWAPCHAIN_H

#include <vulkan/vulkan_raii.hpp>
#include "raii_glfw_window.h"

namespace Bastion
{
  class Swapchain
  {
  private:
    vk::raii::SwapchainKHR swapchain = nullptr;
    vk::Format format = vk::Format::eUndefined;
    vk::Extent2D extent;
    std::vector<vk::Image> images;
    vk::SurfaceFormatKHR surfaceFormat;
    std::vector<vk::raii::ImageView> imageViews;

  public:
    [[nodiscard]] vk::SurfaceFormatKHR& getSurfaceFormat();
    [[nodiscard]] vk::raii::SwapchainKHR& getSwapchain();
    [[nodiscard]] std::vector<vk::Image>& getImages();
    [[nodiscard]] std::vector<vk::raii::ImageView>& getImageViews();
    [[nodiscard]] vk::Extent2D& getExtent();

    [[nodiscard]] vk::SurfaceFormatKHR pickSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats);
    [[nodiscard]] vk::PresentModeKHR pickPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes);
    [[nodiscard]] vk::Extent2D pickExtent(raiiGLFWwindow& window, const vk::SurfaceCapabilitiesKHR& capabilities);

    void createSwapchain(raiiGLFWwindow& window, vk::raii::PhysicalDevice& physicalDevice,
      vk::raii::Device& device, vk::raii::SurfaceKHR& surface);
    void createImageViews(vk::raii::Device& device);
  };
} // Bastion

#endif //BASTION_SWAPCHAIN_H
