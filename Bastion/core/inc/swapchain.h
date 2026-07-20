//
// Created by sjadoona on 2026-06-06.
//

#ifndef BASTION_SWAPCHAIN_H
#define BASTION_SWAPCHAIN_H

#include <vulkan/vulkan_raii.hpp>
#include "device.h"

namespace Bastion
{
  class Swapchain
  {
  private:
    vk::raii::Image image = nullptr;
    vk::raii::DeviceMemory memory = nullptr;
    vk::raii::ImageView imageView = nullptr;
    vk::raii::Image depthImage = nullptr;
    vk::raii::DeviceMemory depthMemory = nullptr;
    vk::raii::ImageView depthImageView = nullptr;
    vk::Format format = vk::Format::eR8G8B8A8Unorm;
    vk::Format depthFormat = vk::Format::eD32Sfloat;
    vk::Extent2D extent;
    uint64_t memorySize = 0;
    int64_t memoryHandle = 0;

  public:
    [[nodiscard]] vk::Format getFormat() const;
    [[nodiscard]] vk::Extent2D& getExtent();
    [[nodiscard]] vk::raii::Image& getImage();
    [[nodiscard]] vk::raii::ImageView& getImageView();
    [[nodiscard]] vk::raii::Image& getDepthImage();
    [[nodiscard]] vk::raii::ImageView& getDepthImageView();
    [[nodiscard]] vk::Format getDepthFormat() const;
    [[nodiscard]] uint64_t getMemorySize() const;
    [[nodiscard]] int64_t getMemoryHandle() const;

    void create(Device& device, uint32_t width, uint32_t height);
  };
} // Bastion

#endif //BASTION_SWAPCHAIN_H
