#include "core/inc/raii_glfw_window.h"
#include "core/inc/instance.h"
#include "core/inc/device.h"
#include "core/inc/pipeline.h"
#include "core/inc/swapchain.h"
#include "core/inc/command_buffer.h"

raiiGLFWwindow window;
Bastion::Instance instance;
Bastion::Device device;
Bastion::Swapchain swapchain;
Bastion::Pipeline pipeline;
Bastion::CommandBuffer commandBuffer;

vk::raii::Semaphore presentSemaphore = nullptr;
vk::raii::Semaphore renderSemaphore = nullptr;
vk::raii::Fence fence = nullptr;

void createSyncObjects()
{
  vk::SemaphoreCreateInfo semaphoreCreateInfo;
  presentSemaphore = vk::raii::Semaphore(device.getDevice(), semaphoreCreateInfo);
  renderSemaphore = vk::raii::Semaphore(device.getDevice(), semaphoreCreateInfo);

  vk::FenceCreateInfo fenceCreateInfo(vk::FenceCreateFlagBits::eSignaled);
  fence = vk::raii::Fence(device.getDevice(), fenceCreateInfo);
}

void initWindow()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

  window = raiiGLFWwindow(glfwCreateWindow(1920, 1080, Bastion::AppInfo::getAppName().c_str(), nullptr, nullptr));

  glfwSetWindowPos(window.get(), 100, 100);
  glfwShowWindow(window.get());
  glfwSetInputMode(window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

  instance.createInstance();
  instance.setupDebugMessenger();
  instance.createSurface(window);
  device.pickPhysicalDevice(instance.getInstance());
  device.createDevice(instance.getSurface());
  swapchain.createSwapchain(window, device.getPhysicalDevice(),
    device.getDevice(), instance.getSurface());
  swapchain.createImageViews(device.getDevice());
  pipeline.setShaderPath("/home/sjadoona/Projects/repo/Bastion/Bastion/shaders/slang.spv");
  pipeline.createPipeline(device.getDevice(), swapchain.getSurfaceFormat());
  commandBuffer.createCommandPool(device.getDevice(), device.getQueueInfo());
  commandBuffer.create(device.getDevice());
  createSyncObjects();
}

void drawFrame()
{
  device.getQueueInfo().queue.waitIdle();

  auto [result, imageIndex] = swapchain.getSwapchain().acquireNextImage(UINT64_MAX, *presentSemaphore);
  commandBuffer.record(swapchain.getExtent(), swapchain.getImages(), swapchain.getImageViews(), pipeline.getPipeline(), imageIndex);
  device.getDevice().resetFences(*fence);

  vk::PipelineStageFlags waitDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
  vk::SubmitInfo submitInfo(
    *presentSemaphore,
    waitDstStageMask,
    *commandBuffer.getCommandBuffer(),
    *renderSemaphore
    );

  device.getQueueInfo().queue.submit(submitInfo, *fence);
  vk::Result waitResult = device.getDevice().waitForFences(*fence, VK_TRUE, UINT64_MAX);
  if (waitResult != vk::Result::eSuccess)
  {
    throw std::runtime_error("waitForFences failed");
  }

  vk::PresentInfoKHR presentInfoKHR(
    *renderSemaphore,
    *swapchain.getSwapchain(),
    imageIndex
  );

  vk::Result presentResult = device.getQueueInfo().queue.presentKHR(presentInfoKHR);
  switch (presentResult)
  {
  case vk::Result::eSuccess:
    break;
  case vk::Result::eSuboptimalKHR:
    break;
  default:
    break;
  }
}

int main()
{
  initWindow();
  while (!glfwWindowShouldClose(window.get()))
  {
    glfwPollEvents();
    drawFrame();
  }
}