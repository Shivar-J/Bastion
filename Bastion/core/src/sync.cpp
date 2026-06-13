//
// Created by sjadoona on 2026-06-08.
//

#include "../inc/sync.h"
#include "../inc/app_info.h"

namespace Bastion
{
  std::vector<vk::raii::Semaphore>& Sync::getPresentSemaphores()
  {
    return presentSemaphores;
  }

  std::vector<vk::raii::Semaphore>& Sync::getRenderSemaphores()
  {
    return renderSemaphores;
  }

  std::vector<vk::raii::Fence>& Sync::getFences()
  {
    return fences;
  }

  void Sync::createSyncObjects(vk::raii::Device& device, const std::vector<vk::Image>& images)
  {
    for (int i =  0; i < images.size(); i++)
    {
      renderSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
    }

    for (int i = 0; i < AppInfo::maxFrameInFlight; i++)
    {
      presentSemaphores.emplace_back(device, vk::SemaphoreCreateInfo());
      fences.emplace_back(device, vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
    }
  }

  void Sync::cleanup()
  {
    presentSemaphores.clear();
    renderSemaphores.clear();
    fences.clear();
  }
} // Bastion