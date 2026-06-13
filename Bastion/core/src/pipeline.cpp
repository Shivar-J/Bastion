//
// Created by sjadoona on 2026-06-07.
//

#include "../inc/pipeline.h"
#include <fstream>

namespace Bastion
{
  vk::raii::ShaderModule Pipeline::createShaderModule(vk::raii::Device& device, const std::vector<char>& code)
  {
    vk::ShaderModuleCreateInfo createInfo(
      {},
      code.size() * sizeof(char),
      reinterpret_cast<const uint32_t*>(code.data())
    );

    vk::raii::ShaderModule module(device, createInfo);
    return module;
  }

  std::vector<char> Pipeline::readFile(const std::string filename)
  {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
      throw std::runtime_error("Failed to open file: " + filename);
    }

    std::vector<char> buf(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buf.data(), static_cast<std::streamsize>(buf.size()));
    file.close();
    return buf;
  }

  vk::raii::Pipeline& Pipeline::getPipeline()
  {
    return pipeline;
  }

  std::string Pipeline::getShaderPath()
  {
    return shaderPath;
  }

  void Pipeline::setShaderPath(const std::string& path)
  {
    shaderPath = path;
  }

  void Pipeline::createPipeline(vk::raii::Device& device, vk::SurfaceFormatKHR& surfaceFormat)
  {
    vk::raii::ShaderModule module = createShaderModule(device, readFile(shaderPath));

    vk::PipelineShaderStageCreateInfo vertStage(
      {},
      vk::ShaderStageFlagBits::eVertex,
      module,
      "vertMain",
      {}, {}
    );

    vk::PipelineShaderStageCreateInfo fragStage(
      {},
      vk::ShaderStageFlagBits::eFragment,
      module,
      "fragMain",
      {}, {}
    );

    vk::PipelineShaderStageCreateInfo shaderStages[] = {
      vertStage,
      fragStage
    };

    vk::PipelineVertexInputStateCreateInfo vertexInputState;
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
      {},
      vk::PrimitiveTopology::eTriangleList,
      {}, nullptr
    );
    vk::PipelineViewportStateCreateInfo viewportState(
      {},
      1, nullptr, 1, nullptr
    );
    vk::PipelineRasterizationStateCreateInfo rasterizerState(
      {}, vk::False, vk::False,
      vk::PolygonMode::eFill,
      vk::CullModeFlagBits::eBack,
      vk::FrontFace::eClockwise,
      vk::False,
      {}, {}, 1.0f, 1.0f,
      nullptr
    );
    vk::PipelineMultisampleStateCreateInfo multisampleState(
      {},
      vk::SampleCountFlagBits::e1,
      vk::False
    );
    vk::PipelineColorBlendAttachmentState colorBlendAttachment(
      vk::False,
      {}, {},{},
      {}, {}, {},
      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    );
    vk::PipelineColorBlendStateCreateInfo blendState(
      {},
      vk::False,
      vk::LogicOp::eCopy,
      1,
      &colorBlendAttachment,
      {},
      nullptr
    );
    std::vector<vk::DynamicState> dynamicStates = {
      vk::DynamicState::eViewport,
      vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicState(
      {},
      static_cast<uint32_t>(dynamicStates.size()),
      dynamicStates.data(),
      nullptr
    );
    vk::PipelineLayoutCreateInfo layout(
      {}, 0, {}, 0, {},
      nullptr
    );
    pipelineLayout = vk::raii::PipelineLayout(device, layout);

    vk::PipelineRenderingCreateInfo pipelineRendering(
      0, 1,
      &surfaceFormat.format
    );

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
      {},
      shaderStages,
      &vertexInputState,
      &inputAssembly,
      nullptr,
      &viewportState,
      &rasterizerState,
      &multisampleState,
      nullptr,
      &blendState,
      &dynamicState,
      *pipelineLayout,
      nullptr
    );

    pipelineCreateInfo.setPNext(&pipelineRendering);
    pipeline = vk::raii::Pipeline(device, nullptr, pipelineCreateInfo);
  }
} // Bastion