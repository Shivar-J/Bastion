cmake_minimum_required(VERSION 4.2)
project(Bastion VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)

find_package(Vulkan REQUIRED)
add_subdirectory(lib/glfw)

add_executable(Bastion Bastion/main.cpp
        Bastion/core/src/instance.cpp
        Bastion/core/inc/instance.h
        Bastion/core/src/device.cpp
        Bastion/core/inc/device.h
        Bastion/core/inc/app_info.h
        Bastion/core/src/app_info.cpp
        Bastion/core/inc/raii_glfw_window.h
        Bastion/core/inc/queue_info.h
        Bastion/core/src/swapchain.cpp
        Bastion/core/inc/swapchain.h
        Bastion/core/src/pipeline.cpp
        Bastion/core/inc/pipeline.h
        Bastion/core/src/command_buffer.cpp
        Bastion/core/inc/command_buffer.h
        Bastion/core/src/sync.cpp
        Bastion/core/inc/sync.h)

target_link_libraries(Bastion Vulkan::Vulkan glfw)
