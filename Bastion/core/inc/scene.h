//
// Created by sjadoona on 2026-06-25.
//

#ifndef BASTION_SCENE_H
#define BASTION_SCENE_H

#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <string>
#include <fstream>
#include "pipeline.h"

namespace Bastion
{
  class Model
  {
  private:
    Pipeline pipeline;
    uint32_t vertexCount = 0;

  public:
    void create(vk::raii::Device& device, vk::Format colorFormat,
      const std::string& shaderPath, uint32_t vertexCount);
    void record(vk::raii::CommandBuffer& cmd, float anim);
  };

  class Scene
  {
  private:
    vk::raii::Device* device = nullptr;
    vk::Format colorFormat = vk::Format::eUndefined;
    std::string assetDir;
    std::vector<std::unique_ptr<Model>> models;

  public:
    void init(vk::raii::Device& device, vk::Format colorFormat, const std::string& assetDir);
    Model& addModel(const std::string& shaderFile, uint32_t vertexCount);
    void record(vk::raii::CommandBuffer& cmd, uint32_t width, uint32_t height, float anim) const;
    void clear();
  };
} // Bastion

#endif //BASTION_SCENE_H
