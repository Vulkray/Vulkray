cmake_minimum_required(VERSION 3.23)
set(name vulkray)
project(${name})

# Using C++20 standard with g++ compiler
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Include Conan Package Manager info
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Source code files
set(sources src/Initialize.cxx
        src/vulkan/VulkanInstance.cxx src/vulkan/VulkanInstance.hxx
        src/vulkan/WindowSurface.cxx src/vulkan/WindowSurface.hxx
        src/vulkan/PhysicalDevice.cxx src/vulkan/PhysicalDevice.hxx
        src/vulkan/LogicalDevice.cxx src/vulkan/LogicalDevice.hxx
        src/vulkan/SwapChain.cxx src/vulkan/SwapChain.hxx
        src/vulkan/ImageViews.cxx src/vulkan/ImageViews.hxx
        src/vulkan/RenderPass.cxx src/vulkan/RenderPass.hxx
        src/vulkan/GraphicsPipeline.cxx src/vulkan/GraphicsPipeline.hxx
        src/vulkan/FrameBuffers.cxx src/vulkan/FrameBuffers.hxx
        src/vulkan/CommandBuffer.cxx src/vulkan/CommandBuffer.hxx
        src/vulkan/Vulkan.cxx src/vulkan/Vulkan.hxx)

# Executable targets
add_executable(${name} ${sources})

# Setup linked libraries using Conan info
target_link_libraries(${name} ${CONAN_LIBS})

# GLSL shaders to Spir-V
add_subdirectory(shaders)
add_dependencies(${name} vulkray-shaders)

# Google Tests
enable_testing()
add_subdirectory(tests)