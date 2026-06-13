//
// Created by sjadoona on 2026-06-06.
//

#ifndef BASTION_APP_INFO_H
#define BASTION_APP_INFO_H

#include <cstdint>
#include <string>
#include <vulkan/vulkan.hpp>

namespace Bastion::AppInfo
{
  inline uint32_t version = vk::makeVersion(1, 0, 0);

  inline std::string appName = "Bastion ver. ";
  inline std::string engineName = "Bastion";

  inline constexpr int maxFrameInFlight = 2;

  [[nodiscard]] std::string getVersion();
  [[nodiscard]] std::string getAppName();
  [[nodiscard]] std::string& getEngineName();
}

#endif //BASTION_APP_INFO_H
