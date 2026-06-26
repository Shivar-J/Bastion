//
// Created by sjadoona on 2026-06-25.
//

#ifndef BASTION_ENGINE_H
#define BASTION_ENGINE_H

#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "command_buffer.h"
#include "sync.h"
#include "scene.h"

namespace Bastion
{
  class Engine
  {
  private:
    Instance instance;
    Device device;
    Swapchain swapchain;
    CommandBuffer commandBuffer;
    Sync sync;
    Scene scene;

    uint32_t width = 0;
    uint32_t height = 0;
    bool initialized = false;
    bool firstFrame = false;
    std::string lastError;

    [[nodiscard]] std::string resolveAssetDir() const;

  public:
    bool init(const uint8_t* idBytes, uint32_t idLen, bool isLuid);
    bool resize(uint32_t newWidth, uint32_t newHeight);
    void render(float anim);
    void shutdown();

    [[nodiscard]] Scene& getScene();

    [[nodiscard]] int64_t getMemoryHandle() const;
    [[nodiscard]] int64_t getWaitSemaphoreHandle() const;
    [[nodiscard]] int64_t getSignalSemaphoreHandle() const;
    [[nodiscard]] uint32_t getWidth() const;
    [[nodiscard]] uint32_t getHeight() const;
    [[nodiscard]] uint64_t getMemorySize() const;
    [[nodiscard]] const char* getLastError() const;
  };
} // Bastion

#endif //BASTION_ENGINE_H
