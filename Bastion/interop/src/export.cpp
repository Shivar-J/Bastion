//
// Created by sjadoona on 2026-06-24.
//

#include <cmath>
#include "../inc/export.h"
#include "../../core/inc/engine.h"

namespace Bastion
{
  namespace
  {
    Engine& engine()
    {
      static Engine instance;
      return instance;
    }
  }

  extern "C"
  {
  BASTION_API bool PreInitWithId(const uint8_t* idBytes, uint32_t idLen, bool isLuid)
  {
    try
    {
      return engine().init(idBytes, idLen, isLuid);
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API bool AddModel(const char* shaderFile, uint32_t vertexCount)
  {
    try
    {
      engine().getScene().addModel(shaderFile ? shaderFile : "", vertexCount);
      return true;
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API bool Resize(uint32_t width, uint32_t height)
  {
    try
    {
      return engine().resize(width, height);
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API void Render()
  {
    try
    {
      engine().render();
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API void HandleUserInput(const UserInput* userInput, int32_t count)
  {
    try
    {
      if (!userInput || count <= 0) return;

      for (int32_t i = 0; i < count; i++)
      {
        engine().addInput(userInput[i]);
      }
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API void GetSharedFrame(SharedFrame* out)
  {
    try
    {
      if (!out) return;
      out->memHandle = engine().getMemoryHandle();
      out->waitSemaphore = engine().getWaitSemaphoreHandle();
      out->signalSemaphore = engine().getSignalSemaphoreHandle();
      out->width = engine().getWidth();
      out->height = engine().getHeight();
      out->format = 0;
      out->memorySize = engine().getMemorySize();
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API void Shutdown()
  {
    try
    {
      engine().shutdown();
    }
    catch (std::exception& ex)
    {
      throw std::runtime_error(ex.what());
    }
  }

  BASTION_API const char* LastError()
  {
    return engine().getLastError();
  }
  }
} // Bastion