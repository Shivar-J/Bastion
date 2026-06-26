//
// Created by sjadoona on 2026-05-27.
//

#ifndef BASTION_DEVICE_H
#define BASTION_DEVICE_H

#include "queue_info.h"

namespace Bastion
{
  class Device
  {
  private:
    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;
    QueueInfo queueInfo;

    std::vector<const char*> requiredDeviceExtensions = {
      vk::KHRExternalMemoryExtensionName,
      vk::KHRExternalSemaphoreExtensionName,
#if defined(_WIN32)
      vk::KHRExternalMemoryWin32ExtensionName,
      vk::KHRExternalSemaphoreWin32ExtensionName
#else
      vk::KHRExternalMemoryFdExtensionName,
      vk::KHRExternalSemaphoreFdExtensionName
#endif
    };

    std::vector<const char*> optionalDeviceExtensions = {

    };

  public:
    [[nodiscard]] vk::raii::PhysicalDevice& getPhysicalDevice();
    [[nodiscard]] vk::raii::Device& getDevice();
    [[nodiscard]] QueueInfo& getQueueInfo();

    void pickPhysicalDevice(const vk::raii::Instance& instance, const uint8_t* id_bytes, uint32_t uint32, bool is_luid);
    void createDevice();
  };
} // Bastion

#endif //BASTION_DEVICE_H
