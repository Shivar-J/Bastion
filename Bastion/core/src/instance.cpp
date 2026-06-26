//
// Created by sjadoona on 2026-05-27.
//

#include "../inc/instance.h"
#include <iostream>

namespace Bastion
{
  vk::raii::Instance& Instance::getInstance()
  {
    return instance;
  }

  void Instance::createInstance()
  {
    uint32_t supportedVersion = context.enumerateInstanceVersion();
    uint32_t apiVersion = std::min(static_cast<uint32_t>(vk::ApiVersion14), supportedVersion);

    vk::ApplicationInfo appInfo(AppInfo::getAppName().c_str(),
      AppInfo::version,
      AppInfo::getEngineName().c_str(),
      AppInfo::version,
      apiVersion);

    std::vector<const char*> extensions;
    extensions.push_back(vk::KHRGetPhysicalDeviceProperties2ExtensionName);
    extensions.push_back(vk::KHRExternalMemoryCapabilitiesExtensionName);
    extensions.push_back(vk::KHRExternalSemaphoreCapabilitiesExtensionName);
#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    std::vector<const char*> validationLayers = {
      "VK_LAYER_KHRONOS_validation"
    };

    vk::InstanceCreateInfo createInfo(
      {},
      &appInfo,
      validationLayers,
      extensions
      );
#else
    vk::InstanceCreateInfo createInfo(
      {},
      &appInfo,
      {},
      extensions
      );
#endif

    instance = vk::raii::Instance(context, createInfo);
  }

  void Instance::setupDebugMessenger()
  {
#ifdef _DEBUG
    vk::DebugUtilsMessageSeverityFlagBitsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    vk::DebugUtilsMessengerCreateInfoEXT createInfo(
      {},
      severityFlags,
      messageTypeFlags,
      debugCallback
      );

    debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
#endif
  }

  vk::Bool32 Instance::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
  {
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError ||
      severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
      std::cerr << "validationlayer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
  }
} // Bastion