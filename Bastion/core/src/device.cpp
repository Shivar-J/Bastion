//
// Created by sjadoona on 2026-05-27.
//

#include "../inc/device.h"
#include <algorithm>

namespace Bastion
{
  vk::raii::PhysicalDevice& Device::getPhysicalDevice()
  {
    return physicalDevice;
  }

  vk::raii::Device& Device::getDevice()
  {
    return device;
  }

  QueueInfo& Device::getQueueInfo()
  {
    return queueInfo;
  }

  void Device::pickPhysicalDevice(const vk::raii::Instance& instance)
  {
    std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();
    const auto devIter = std::ranges::find_if(devices, [&](auto const& _device)
    {
      const bool supportsVulkan13 = _device.getProperties().apiVersion >= vk::ApiVersion13;
      auto queueFamilies = _device.getQueueFamilyProperties();
      const bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp)
      {
        return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
      });

      auto availableDeviceExtensions = _device.enumerateDeviceExtensionProperties();
      const bool supportsAllRequiredExtensions = std::ranges::all_of(requiredDeviceExtensions,
        [&availableDeviceExtensions](auto const& requiredDeviceExtension)
      {
        return std::ranges::any_of(availableDeviceExtensions,
          [requiredDeviceExtension](auto const& availableDeviceExtension)
        {
          return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0;
        });
      });

      auto features = _device.template getFeatures2<vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
      const bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering
        && features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

      return supportsVulkan13 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
    });
    if (devIter != devices.end())
    {
      physicalDevice = *devIter;
    }
    else
    {
      throw std::runtime_error("Failed to find suitable GPU");
    }
  }

  void Device::createDevice(vk::raii::SurfaceKHR& surface)
  {
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    uint32_t queueIndex = ~0;
    for (uint32_t i = 0; i < queueFamilyProperties.size(); i++)
    {
      if ((queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
        && physicalDevice.getSurfaceSupportKHR(i, surface))
      {
        queueIndex = i;
        break;
      }
    }

    if (queueIndex == ~0)
    {
      throw std::runtime_error("Failed to find queue");
    }

    vk::PhysicalDeviceFeatures2 physicalDeviceFeatures2{};
    vk::PhysicalDeviceVulkan11Features physicalDeviceVulkan11Features{};
    physicalDeviceVulkan11Features.shaderDrawParameters = vk::True;
    vk::PhysicalDeviceVulkan13Features physicalDeviceVulkan13Features{};
    physicalDeviceVulkan13Features.dynamicRendering = vk::True;
    vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT extendedDynamicStateFeatures{};
    extendedDynamicStateFeatures.extendedDynamicState = vk::True;

    vk::StructureChain<
      vk::PhysicalDeviceFeatures2,
      vk::PhysicalDeviceVulkan11Features,
      vk::PhysicalDeviceVulkan13Features,
      vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
      > featureChain(physicalDeviceFeatures2,
        physicalDeviceVulkan11Features,
        physicalDeviceVulkan13Features,
        extendedDynamicStateFeatures);

    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo(
      {},
      queueIndex,
      1,
      &queuePriority
    );
    vk::DeviceCreateInfo deviceCreateInfo(
      {},
      deviceQueueCreateInfo,
      {},
      requiredDeviceExtensions
    );
    deviceCreateInfo.setPNext(&featureChain.get<vk::PhysicalDeviceFeatures2>());

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queueInfo.queue = vk::raii::Queue(device, queueIndex, 0);
    queueInfo.index = queueIndex;
  }
} // Bastion