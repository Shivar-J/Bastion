//
// Created by sjadoona on 2026-06-07.
//

#include "../inc/command_buffer.h"

namespace Bastion
{
  vk::raii::CommandPool& CommandBuffer::getCommandPool()
  {
    return commandPool;
  }

  vk::raii::CommandBuffer& CommandBuffer::getCommandBuffer()
  {
    return commandBuffer;
  }

  void CommandBuffer::createCommandPool(const vk::raii::Device& device, const QueueInfo& queue)
  {
    vk::CommandPoolCreateInfo commandPoolCreateInfo(
      vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
      queue.index
    );

    commandPool = vk::raii::CommandPool(device, commandPoolCreateInfo);
  }

  void CommandBuffer::transitionImageLayout(const std::vector<vk::Image>& images, const uint32_t imageIndex,
    const vk::ImageLayout oldLayout, const vk::ImageLayout newLayout, const vk::AccessFlags2 srcAccessMask,
    const vk::AccessFlags2 dstAccessMask, const vk::PipelineStageFlags2 srcStageMask, const vk::PipelineStageFlags2 dstStageMask) const
  {
    vk::ImageMemoryBarrier2 barrier(
      srcStageMask,
      srcAccessMask,
      dstStageMask,
      dstAccessMask,
      oldLayout,
      newLayout,
      VK_QUEUE_FAMILY_IGNORED,
      VK_QUEUE_FAMILY_IGNORED,
      images[imageIndex],
      vk::ImageSubresourceRange(
        vk::ImageAspectFlagBits::eColor,
        0, 1, 0, 1
      )
    );

    vk::DependencyInfo dependencyInfo(
      {}, {}, {}, {}, {}, 1,
      &barrier
    );

    commandBuffer.pipelineBarrier2(dependencyInfo);
  }

  void CommandBuffer::create(const vk::raii::Device& device)
  {
    const vk::CommandBufferAllocateInfo allocInfo(
      commandPool,
      vk::CommandBufferLevel::ePrimary,
      1
    );

    commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());
  }

  void CommandBuffer::record(vk::Extent2D& extent, std::vector<vk::Image>& images,
                             std::vector<vk::raii::ImageView>& imageViews, vk::raii::Pipeline& pipeline, uint32_t imageIndex)
  {
    commandBuffer.begin({});

    transitionImageLayout(images,
      imageIndex, vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      {},
      vk::AccessFlagBits2::eColorAttachmentWrite,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput
      );

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo(
      imageViews[imageIndex],
      vk::ImageLayout::eColorAttachmentOptimal,
      {}, {}, {},
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore,
      clearColor
    );

    vk::RenderingInfo renderingInfo(
      {},
      vk::Rect2D(
        {0, 0},
        extent
      ),
      1, 0, 1,
      &attachmentInfo,
      nullptr, nullptr
    );

    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
      static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent));
    commandBuffer.draw(3, 1, 0, 0);
    commandBuffer.endRendering();

    transitionImageLayout(images, imageIndex,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::ePresentSrcKHR,
      vk::AccessFlagBits2::eColorAttachmentWrite,
      {},
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::PipelineStageFlagBits2::eBottomOfPipe
    );

    commandBuffer.end();
  }
} // Bastion