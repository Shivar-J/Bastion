cmake_minimum_required(VERSION 4.2)
project(Bastion VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

find_package(Vulkan REQUIRED)

add_library(Bastion SHARED
        Bastion/main.cpp
        Bastion/core/src/instance.cpp
        Bastion/core/inc/instance.h
        Bastion/core/src/device.cpp
        Bastion/core/inc/device.h
        Bastion/core/inc/app_info.h
        Bastion/core/src/app_info.cpp
        Bastion/core/inc/queue_info.h
        Bastion/core/src/swapchain.cpp
        Bastion/core/inc/swapchain.h
        Bastion/core/src/pipeline.cpp
        Bastion/core/inc/pipeline.h
        Bastion/core/src/command_buffer.cpp
        Bastion/core/inc/command_buffer.h
        Bastion/core/src/sync.cpp
        Bastion/core/inc/sync.h
        Bastion/interop/src/export.cpp
        Bastion/interop/inc/export.h
        Bastion/core/src/scene.cpp
        Bastion/core/inc/scene.h
        Bastion/core/src/engine.cpp
        Bastion/core/inc/engine.h
        Bastion/core/inc/user_input.h)

add_custom_target(shaders ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Bastion/shaders/" "$<TARGET_FILE_DIR:Bastion>/shaders")

add_dependencies(Bastion shaders)

target_link_libraries(Bastion Vulkan::Vulkan)

