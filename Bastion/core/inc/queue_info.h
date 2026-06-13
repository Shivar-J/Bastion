//
// Created by sjadoona on 2026-06-06.
//

#ifndef BASTION_QUEUE_INFO_H
#define BASTION_QUEUE_INFO_H

#include <vulkan/vulkan_raii.hpp>
#include <stdint.h>

namespace Bastion
{
  struct QueueInfo
  {
    vk::raii::Queue queue = nullptr;
    uint32_t index = 0;
  };
}

#endif //BASTION_QUEUE_INFO_H
