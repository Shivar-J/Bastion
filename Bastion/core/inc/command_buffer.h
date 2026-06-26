//
// Created by sjadoona on 2026-06-07.
//

#ifndef BASTION_COMMAND_BUFFER_H
#define BASTION_COMMAND_BUFFER_H

#include <vulkan/vulkan_raii.hpp>
#include "device.h"
#include "scene.h"

namespace Bastion
{
  class CommandBuffer
  {
  private:
    vk::raii::CommandPool commandPool = nullptr;
    vk::raii::CommandBuffer commandBuffer = nullptr;

  public:
    [[nodiscard]] vk::raii::CommandPool& getCommandPool();
    [[nodiscard]] vk::raii::CommandBuffer& getCommandBuffer();

    void createCommandPool(const vk::raii::Device& device, const QueueInfo& queue);
    void transitionImageLayout(vk::Image image,
      vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask,
      vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask) const;
    void create(const vk::raii::Device& device);
    void record(vk::Extent2D& extent, vk::Image image, vk::raii::ImageView& imageView,
      Scene& scene, float anim);

  };
} // Bastion

#endif //BASTION_COMMAND_BUFFER_H
