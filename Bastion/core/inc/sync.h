//
// Created by sjadoona on 2026-06-08.
//

#ifndef BASTION_SYNC_H
#define BASTION_SYNC_H

#include <vulkan/vulkan_raii.hpp>

namespace Bastion
{
  class Sync
  {
  private:
    std::vector<vk::raii::Semaphore> presentSemaphores;
    std::vector<vk::raii::Semaphore> renderSemaphores;
    std::vector<vk::raii::Fence> fences;

  public:
    [[nodiscard]] std::vector<vk::raii::Semaphore>& getPresentSemaphores();
    [[nodiscard]] std::vector<vk::raii::Semaphore>& getRenderSemaphores();
    [[nodiscard]] std::vector<vk::raii::Fence>& getFences();

    void createSyncObjects(vk::raii::Device& device, const std::vector<vk::Image>& images);
    void cleanup();
  };
} // Bastion

#endif //BASTION_SYNC_H
