//
// Created by sjadoona on 2026-06-08.
//

#include "../inc/sync.h"
#include "../inc/app_info.h"

namespace Bastion
{
#if defined(_WIN32)
  static constexpr vk::ExternalSemaphoreHandleTypeFlagBits kSemaphoreHandleType =
    vk::ExternalSemaphoreHandleTypeFlagBits::eOpaqueWin32;
#else
  static constexpr vk::ExternalSemaphoreHandleTypeFlagBits kSemaphoreHandleType =
    vk::ExternalSemaphoreHandleTypeFlagBits::eOpaqueFd;
#endif

  vk::raii::Semaphore& Sync::getRenderFinished()
  {
    return renderFinished;
  }

  vk::raii::Semaphore& Sync::getImageAvailable()
  {
    return imageAvailable;
  }

  vk::raii::Fence& Sync::getFence()
  {
    return fence;
  }

  int64_t Sync::getRenderFinishedHandle() const
  {
    return renderFinishedHandle;
  }

  int64_t Sync::getImageAvailableHandle() const
  {
    return imageAvailableHandle;
  }

  void Sync::create(vk::raii::Device& device)
  {
    auto makeExportable = [&](vk::raii::Semaphore& semaphore, int64_t& handle)
    {
      vk::ExportSemaphoreCreateInfo exportInfo(kSemaphoreHandleType);
      vk::SemaphoreCreateInfo semaphoreInfo;
      semaphoreInfo.setPNext(&exportInfo);
      semaphore = vk::raii::Semaphore(device, semaphoreInfo);

#if defined(_WIN32)
      vk::SemaphoreGetWin32HandleInfoKHR handleInfo(*semaphore, kSemaphoreHandleType);
      handle = reinterpret_cast<int64_t>(device.getSemaphoreWin32HandleKHR(handleInfo));
#else
      vk::SemaphoreGetFdInfoKHR handleInfo(*semaphore, kSemaphoreHandleType);
      handle = static_cast<int64_t>(device.getSemaphoreFdKHR(handleInfo));
#endif
    };

    makeExportable(renderFinished, renderFinishedHandle);
    makeExportable(imageAvailable, imageAvailableHandle);

    fence = vk::raii::Fence(device, vk::FenceCreateInfo());
  }

  void Sync::cleanup()
  {
    renderFinished = nullptr;
    imageAvailable = nullptr;
    fence = nullptr;
    renderFinishedHandle = 0;
    imageAvailableHandle = 0;
  }
} // Bastion