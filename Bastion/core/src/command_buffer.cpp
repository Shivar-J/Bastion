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

  void CommandBuffer::transitionImageLayout(const vk::Image image,
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
      image,
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

  void CommandBuffer::record(const vk::Extent2D& extent, const vk::Image image, const vk::raii::ImageView& imageView,
                             vk::Image depthImage, const vk::raii::ImageView& depthImageView,
                             const Scene& scene, const Camera& camera)
  {
    commandBuffer.begin({});

    transitionImageLayout(image,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eColorAttachmentOptimal,
      {},
      vk::AccessFlagBits2::eColorAttachmentWrite,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::PipelineStageFlagBits2::eColorAttachmentOutput
      );

    vk::ImageMemoryBarrier2 depthBarrier(
      vk::PipelineStageFlagBits2::eTopOfPipe,
      {},
      vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
      vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
      vk::ImageLayout::eUndefined,
      vk::ImageLayout::eDepthAttachmentOptimal,
      vk::QueueFamilyIgnored,
      vk::QueueFamilyIgnored,
      depthImage,
      vk::ImageSubresourceRange(
        vk::ImageAspectFlagBits::eDepth,
        0, 1, 0, 1
        )
      );

    commandBuffer.pipelineBarrier2(vk::DependencyInfo({}, {}, {}, depthBarrier));

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo(
      imageView,
      vk::ImageLayout::eColorAttachmentOptimal,
      {}, {}, {},
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eStore,
      clearColor
    );

    vk::ClearValue depthClear = vk::ClearDepthStencilValue(1.0f, 0);
    vk::RenderingAttachmentInfo depthAttachmentInfo(
      depthImageView,
      vk::ImageLayout::eDepthAttachmentOptimal,
      {}, {}, {},
      vk::AttachmentLoadOp::eClear,
      vk::AttachmentStoreOp::eDontCare,
      depthClear
    );
    const vk::RenderingInfo renderingInfo(
      {},
      vk::Rect2D(
        {0, 0},
        extent
      ),
      1, 0, 1,
      &attachmentInfo,
      &depthAttachmentInfo,
      nullptr
    );

    commandBuffer.beginRendering(renderingInfo);
    commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f,
      static_cast<float>(extent.width), static_cast<float>(extent.height), 0.0f, 1.0f));
    commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), extent));
    scene.record(commandBuffer, extent.width, extent.height, camera);
    commandBuffer.endRendering();

    transitionImageLayout(image,
      vk::ImageLayout::eColorAttachmentOptimal,
      vk::ImageLayout::eShaderReadOnlyOptimal,
      vk::AccessFlagBits2::eColorAttachmentWrite,
      {},
      vk::PipelineStageFlagBits2::eColorAttachmentOutput,
      vk::PipelineStageFlagBits2::eBottomOfPipe
    );

    commandBuffer.end();
  }
} // Bastion