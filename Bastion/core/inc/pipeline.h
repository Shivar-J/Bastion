//
// Created by sjadoona on 2026-06-07.
//

#ifndef BASTION_PIPELINE_H
#define BASTION_PIPELINE_H

#include <vulkan/vulkan_raii.hpp>

namespace Bastion
{
  class Pipeline
  {
  private:
    vk::raii::PipelineLayout pipelineLayout = nullptr;
    vk::raii::Pipeline pipeline = nullptr;
    std::string shaderPath = "";

    [[nodiscard]] vk::raii::ShaderModule createShaderModule(vk::raii::Device& device, const std::vector<char>& code);
    [[nodiscard]] std::vector<char> readFile(const std::string filename);
  public:
    [[nodiscard]] vk::raii::Pipeline& getPipeline();
    [[nodiscard]] std::string getShaderPath();

    void setShaderPath(const std::string& path);
    void createPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat);
  };
} // Bastion

#endif //BASTION_PIPELINE_H
