//
// Created by sjadoona on 2026-06-06.
//

#include "../inc/swapchain.h"
#include <algorithm>

namespace Bastion
{
  vk::SurfaceFormatKHR& Swapchain::getSurfaceFormat()
  {
    return surfaceFormat;
  }

  vk::raii::SwapchainKHR& Swapchain::getSwapchain()
  {
    return swapchain;
  }

  std::vector<vk::Image>& Swapchain::getImages()
  {
    return images;
  }

  std::vector<vk::raii::ImageView>& Swapchain::getImageViews()
  {
    return imageViews;
  }

  vk::Extent2D& Swapchain::getExtent()
  {
    return extent;
  }

  vk::SurfaceFormatKHR Swapchain::pickSurfaceFormat(std::vector<vk::SurfaceFormatKHR> availableFormats)
  {
    const auto formatIter = std::ranges::find_if(availableFormats, [](const auto& _format)
    {
      return _format.format == vk::Format::eR8G8B8A8Srgb && _format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });

    return formatIter != availableFormats.end() ? *formatIter : availableFormats[0];
  }

  vk::PresentModeKHR Swapchain::pickPresentMode(std::vector<vk::PresentModeKHR> availablePresentModes)
  {
    return std::ranges::any_of(availablePresentModes, [](const auto value)
    {
      return vk::PresentModeKHR::eMailbox == value;
    }) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
  }

  vk::Extent2D Swapchain::pickExtent(raiiGLFWwindow& window, const vk::SurfaceCapabilitiesKHR& capabilities)
  {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
      return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window.get(), &width, &height);

    return {
      std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
      std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
  }

  void Swapchain::createSwapchain(raiiGLFWwindow& window, vk::raii::PhysicalDevice& physicalDevice,
    vk::raii::Device& device, vk::raii::SurfaceKHR& surface)
  {
    auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
    extent = pickExtent(window, surfaceCapabilities);
    surfaceFormat = pickSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(*surface));

    uint32_t minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
    {
      minImageCount = surfaceCapabilities.maxImageCount;
    }

    auto presentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
    auto presentMode = pickPresentMode(presentModes);

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
      {},
      *surface,
      minImageCount,
      surfaceFormat.format,
      surfaceFormat.colorSpace,
      extent,
      1,
      vk::ImageUsageFlagBits::eColorAttachment,
      vk::SharingMode::eExclusive,
      {},
      surfaceCapabilities.currentTransform,
      vk::CompositeAlphaFlagBitsKHR::eOpaque,
      presentMode,
      true
    );

    swapchain = vk::raii::SwapchainKHR(device, swapchainCreateInfo);
    images = swapchain.getImages();
  }

  void Swapchain::createImageViews(vk::raii::Device& device)
  {
    vk::ImageViewCreateInfo createInfo(
      {}, {},
      vk::ImageViewType::e2D,
      surfaceFormat.format,
      {},
      vk::ImageSubresourceRange(
        vk::ImageAspectFlagBits::eColor,
        0, 1, 0, 1
      )
    );

    for (auto& image : images)
    {
      createInfo.image = image;
      imageViews.emplace_back(device, createInfo);
    }
  }
} // Bastion