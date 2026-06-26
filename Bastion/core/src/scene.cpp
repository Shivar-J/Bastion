//
// Created by sjadoona on 2026-06-25.
//

#include "../inc/scene.h"

namespace Bastion
{
  void Model::create(vk::raii::Device& device, vk::Format colorFormat,
    const std::string& shaderPath, uint32_t vertexCount)
  {
    this->vertexCount = vertexCount;
    pipeline.setShaderPath(shaderPath);
    pipeline.createPipeline(device, colorFormat);
  }

  void Model::record(vk::raii::CommandBuffer& cmd)
  {
    cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline.getPipeline());
    cmd.draw(vertexCount, 1, 0, 0);
  }

  void Scene::init(vk::raii::Device& _device, vk::Format _colorFormat, const std::string& _assetDir)
  {
    device = &_device;
    colorFormat = _colorFormat;
    assetDir = _assetDir;
  }

  Model& Scene::addModel(const std::string& shaderFile, uint32_t vertexCount)
  {
    auto model = std::make_unique<Model>();
    model->create(*device, colorFormat, assetDir + shaderFile, vertexCount);
    models.push_back(std::move(model));
    return *models.back();
  }

  void Scene::record(vk::raii::CommandBuffer& cmd, uint32_t, uint32_t, float) const
  {
    for (const auto& model : models)
    {
      model->record(cmd);
    }
  }

  void Scene::clear()
  {
    models.clear();
  }
} // Bastion