//
// Created by sjadoona on 2026-06-06.
//

#include "../inc/app_info.h"

namespace Bastion::AppInfo
{
  std::string getVersion()
  {
    return std::to_string(VK_VERSION_MAJOR(version)) +
      "." + std::to_string(VK_VERSION_MINOR(version)) +
      "." + std::to_string(VK_VERSION_PATCH(version));
  }

  std::string getAppName()
  {
    return appName + getVersion();
  }

  std::string& getEngineName()
  {
    return engineName;
  }
}
