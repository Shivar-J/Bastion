//
// Created by sjadoona on 2026-06-06.
//

#include "../inc/swapchain.h"
#include <algorithm>

namespace Bastion
{
#if defined(_WIN32)
  static constexpr vk::ExternalMemoryHandleTypeFlagBits kMemoryHandleType =
    vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32;
#else
  static constexpr vk::ExternalMemoryHandleTypeFlagBits kMemoryHandleType =
    vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd;
#endif

  vk::Format Swapchain::getFormat() const
  {
    return format;
  }

  vk::Extent2D& Swapchain::getExtent()
  {
    return extent;
  }

  vk::raii::Image& Swapchain::getImage()
  {
    return image;
  }

  vk::raii::ImageView& Swapchain::getImageView()
  {
    return imageView;
  }

  uint64_t Swapchain::getMemorySize() const
  {
    return memorySize;
  }

  int64_t Swapchain::getMemoryHandle() const
  {
    return memoryHandle;
  }

  void Swapchain::create(Device& device, uint32_t width, uint32_t height)
  {
    extent = vk::Extent2D(width, height);

    vk::ExternalMemoryImageCreateInfo externalImageInfo(kMemoryHandleType);
    vk::ImageCreateInfo imageInfo(
      vk::ImageCreateFlagBits::eMutableFormat,
      vk::ImageType::e2D,
      format,
      vk::Extent3D(width, height, 1),
      1, 1,
      vk::SampleCountFlagBits::e1,
      vk::ImageTiling::eOptimal,
      vk::ImageUsageFlagBits::eColorAttachment |
      vk::ImageUsageFlagBits::eTransferDst |
      vk::ImageUsageFlagBits::eTransferSrc |
      vk::ImageUsageFlagBits::eSampled,
      vk::SharingMode::eExclusive
    );
    imageInfo.setPNext(&externalImageInfo);
    image = vk::raii::Image(device.getDevice(), imageInfo);

    vk::MemoryRequirements requirements = image.getMemoryRequirements();
    memorySize = requirements.size;

    vk::PhysicalDeviceMemoryProperties memoryProperties = device.getPhysicalDevice().getMemoryProperties();
    uint32_t memoryTypeIndex = ~0u;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
      if ((requirements.memoryTypeBits & (1u << i)) &&
        (memoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal))
      {
        memoryTypeIndex = i;
        break;
      }
    }
    if (memoryTypeIndex == ~0u)
    {
      throw std::runtime_error("No suitable memory type for offscreen image");
    }

    vk::ExportMemoryAllocateInfo exportInfo(kMemoryHandleType);
    vk::MemoryDedicatedAllocateInfo dedicatedInfo(*image);
    dedicatedInfo.setPNext(&exportInfo);
    vk::MemoryAllocateInfo allocateInfo(requirements.size, memoryTypeIndex);
    allocateInfo.setPNext(&dedicatedInfo);

    memory = vk::raii::DeviceMemory(device.getDevice(), allocateInfo);
    image.bindMemory(*memory, 0);

#if defined(_WIN32)
    vk::MemoryGetWin32HandleInfoKHR handleInfo(*memory, kMemoryHandleType);
    memoryHandle = reinterpret_cast<int64_t>(device.getDevice().getMemoryWin32HandleKHR(handleInfo));
#else
    vk::MemoryGetFdInfoKHR handleInfo(*memory, kMemoryHandleType);
    memoryHandle = static_cast<int64_t>(device.getDevice().getMemoryFdKHR(handleInfo));
#endif

    vk::ImageViewCreateInfo viewInfo(
      {},
      *image,
      vk::ImageViewType::e2D,
      format,
      {},
      vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    );
    imageView = vk::raii::ImageView(device.getDevice(), viewInfo);
  }
} // Bastion