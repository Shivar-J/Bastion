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
    vk::raii::Semaphore renderFinished = nullptr;
    vk::raii::Semaphore imageAvailable = nullptr;
    vk::raii::Fence fence = nullptr;
    int64_t renderFinishedHandle = 0;
    int64_t imageAvailableHandle = 0;

  public:
    [[nodiscard]] vk::raii::Semaphore& getRenderFinished();
    [[nodiscard]] vk::raii::Semaphore& getImageAvailable();
    [[nodiscard]] vk::raii::Fence& getFence();
    [[nodiscard]] int64_t getRenderFinishedHandle() const;
    [[nodiscard]] int64_t getImageAvailableHandle() const;

    void create(vk::raii::Device& device);
    void cleanup();
  };
} // Bastion

#endif //BASTION_SYNC_H
