//
// Created by sjadoona on 2026-06-25.
//

#include "../inc/engine.h"

#include <fstream>
#include <stdexcept>

#if defined(_WIN32)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <limits.h>
  #include <unistd.h>
#endif

namespace Bastion
{
  namespace
  {
    std::string moduleDirectory()
    {
#if defined(_WIN32)
      HMODULE module = nullptr;
      GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCSTR>(&moduleDirectory), &module);
      char buffer[MAX_PATH] = {0};
      GetModuleFileNameA(module, buffer, MAX_PATH);
      std::string path(buffer);
      size_t slash = path.find_last_of("\\/");
      return slash == std::string::npos ? std::string() : path.substr(0, slash + 1);
#else
      Dl_info info;
      if (dladdr(reinterpret_cast<void*>(&moduleDirectory), &info) && info.dli_fname)
      {
        std::string path(info.dli_fname);
        size_t slash = path.find_last_of('/');
        if (slash != std::string::npos) return path.substr(0, slash + 1);
      }
      char buffer[PATH_MAX];
      ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
      if (count <= 0) return std::string();
      buffer[count] = '\0';
      std::string path(buffer);
      size_t slash = path.find_last_of('/');
      return slash == std::string::npos ? std::string() : path.substr(0, slash + 1);
#endif
    }
  }

  std::string Engine::resolveAssetDir() const
  {
    std::string dir = moduleDirectory();
    const char* candidates[] = { "", "shaders/", "../shaders/" };
    for (const char* candidate : candidates)
    {
      std::string path = dir + candidate + "slang.spv";
      std::ifstream probe(path, std::ios::binary);
      if (probe.is_open()) return dir + candidate;
    }
    return dir;
  }

  Scene& Engine::getScene()
  {
    return scene;
  }

  bool Engine::init(const uint8_t* idBytes, uint32_t idLen, bool isLuid)
  {
    try
    {
      lastError.clear();
      if (initialized) return true;

      instance.createInstance();
      instance.setupDebugMessenger();
      device.pickPhysicalDevice(instance.getInstance(), idBytes, idLen, isLuid);
      device.createDevice();

      commandBuffer.createCommandPool(device.getDevice(), device.getQueueInfo());
      commandBuffer.create(device.getDevice());

      scene.init(device.getDevice(), swapchain.getFormat(), resolveAssetDir());

      initialized = true;
      return true;
    }
    catch (const std::exception& e)
    {
      lastError = e.what();
      return false;
    }
  }

  bool Engine::resize(uint32_t newWidth, uint32_t newHeight)
  {
    try
    {
      if (!initialized) throw std::runtime_error("Engine::init not called");
      lastError.clear();

      device.getDevice().waitIdle();
      width = newWidth;
      height = newHeight;

      swapchain.create(device, newWidth, newHeight);
      sync.create(device.getDevice());
      firstFrame = true;
      return true;
    }
    catch (const std::exception& e)
    {
      lastError = e.what();
      return false;
    }
  }

  void Engine::render()
  {
    if (!initialized) return;

    input.process(camera);
    commandBuffer.record(swapchain.getExtent(), *swapchain.getImage(),
                         swapchain.getImageView(), scene, camera);

    device.getDevice().resetFences(*sync.getFence());

    vk::PipelineStageFlags waitStage(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    vk::SubmitInfo submitInfo;
    if (!firstFrame)
    {
      submitInfo.setWaitSemaphores(*sync.getImageAvailable());
      submitInfo.setWaitDstStageMask(waitStage);
    }
    submitInfo.setCommandBuffers(*commandBuffer.getCommandBuffer());
    submitInfo.setSignalSemaphores(*sync.getRenderFinished());

    device.getQueueInfo().queue.submit(submitInfo, *sync.getFence());
    vk::Result waitResult = device.getDevice().waitForFences(*sync.getFence(), vk::True, UINT64_MAX);
    if (waitResult != vk::Result::eSuccess)
    {
      lastError = "waitForFences failed";
    }
    firstFrame = false;
  }

  void Engine::shutdown()
  {
    if (device.getDevice() != nullptr)
    {
      device.getDevice().waitIdle();
    }
    scene.clear();
    sync.cleanup();
    initialized = false;
  }

  void Engine::addInput(const UserInput& _input)
  {
    input.add(_input);
  }

  int64_t Engine::getMemoryHandle() const
  {
    return swapchain.getMemoryHandle();
  }

  int64_t Engine::getWaitSemaphoreHandle() const
  {
    return sync.getImageAvailableHandle();
  }

  int64_t Engine::getSignalSemaphoreHandle() const
  {
    return sync.getRenderFinishedHandle();
  }

  uint32_t Engine::getWidth() const
  {
    return width;
  }

  uint32_t Engine::getHeight() const
  {
    return height;
  }

  uint64_t Engine::getMemorySize() const
  {
    return swapchain.getMemorySize();
  }

  const char* Engine::getLastError() const
  {
    return lastError.c_str();
  }
} // Bastion