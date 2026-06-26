#include "./core/inc/engine.h"
#include <cstdio>
#include <iostream>

int main()
{
  try
  {
    Bastion::Engine engine;

    if (!engine.init(nullptr, 0, false))
    {
      std::fprintf(stderr, "init failed: %s\n", engine.getLastError());
      return 1;
    }

    if (!engine.resize(1920, 1080))
    {
      std::fprintf(stderr, "resize failed: %s\n", engine.getLastError());
      engine.shutdown();
      return 1;
    }

    engine.getScene().addModel("slang.spv", 3);

    std::printf("shared frame: mem=%lld wait=%lld signal=%lld %ux%u memorySize=%llu\n",
      static_cast<long long>(engine.getMemoryHandle()),
      static_cast<long long>(engine.getWaitSemaphoreHandle()),
      static_cast<long long>(engine.getSignalSemaphoreHandle()),
      engine.getWidth(), engine.getHeight(),
      static_cast<unsigned long long>(engine.getMemorySize()));

    for (int i = 0; i < 60; i++)
    {
      std::cout << i << std::endl;
      engine.render(static_cast<float>(i));
    }

    engine.shutdown();
    return 0;
  }
  catch (std::exception& e)
  {
    std::fprintf(stderr, "exception: %s\n", e.what());
  }
}
