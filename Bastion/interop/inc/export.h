//
// Created by sjadoona on 2026-06-24.
//

#ifndef BASTION_EXPORT_H
#define BASTION_EXPORT_H

#include <cstdint>

#if defined(_WIN32)
#define BASTION_API __declspec(dllexport)
#else
#define BASTION_API __attribute__((visibility("default")))
#endif

namespace Bastion
{
#pragma pack(push, 1)
  typedef struct SharedFrame
  {
    int64_t memHandle;
    int64_t waitSemaphore;
    int64_t signalSemaphore;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t memorySize;
  } SharedFrame;
#pragma pack(pop)
#ifdef __cplusplus
  extern "C" {
#endif
  BASTION_API bool PreInitWithId(const uint8_t* idBytes, uint32_t idLen, bool isLuid);
  BASTION_API bool AddModel(const char* shaderFile, uint32_t vertexCount);
  BASTION_API bool Resize(uint32_t width, uint32_t height);
  BASTION_API void Render(float anim);
  BASTION_API void GetSharedFrame(SharedFrame* out);
  BASTION_API void Shutdown();
  BASTION_API const char* LastError();
#ifdef __cplusplus
  }
#endif
} // Bastion

#endif //BASTION_EXPORT_H
