//
// Created by sjadoona on 2026-06-07.
//

#ifndef BASTION_COMMAND_BUFFER_H
#define BASTION_COMMAND_BUFFER_H

#include <vulkan/vulkan_raii.hpp>
#include "device.h"

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
    void transitionImageLayout(const std::vector<vk::Image>& images, uint32_t imageIndex,
      vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask,
      vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask, vk::PipelineStageFlags2 dstStageMask) const;
    void create(const vk::raii::Device& device);
    void record(vk::Extent2D& extent, std::vector<vk::Image>& images, std::vector<vk::raii::ImageView>& imageViews,
      vk::raii::Pipeline& pipeline, uint32_t imageIndex);

  };
} // Bastion

#endif //BASTION_COMMAND_BUFFER_H
