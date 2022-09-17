cmake_minimum_required(VERSION 3.22)
set(name vulkray)
project(${name})

# Using C++20 standard with g++ compiler
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
# Compiler flags (`-Wno-error=volatile` due to GLM headers. Updating GLM ASAP)
set(WARNING_DISABLE_FLAGS "-Wno-error=volatile -Wno-error=unused-variable -Wno-error=parentheses")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fPIC ${WARNING_DISABLE_FLAGS}")

# CMake Build Options
option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)

# Enable GCC/clang ANSI-colored terminal output using Ninja build tool
if (FORCE_COLORED_OUTPUT)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

# Include Conan Package Manager info
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# Source code files
set(sources src/global_definitions.h
        src/vulkan/Vulkan.h src/EngineBase.cxx
        src/vulkan/VulkanInstance.cxx src/vulkan/Window.cxx
        src/vulkan/PhysicalDevice.cxx src/vulkan/LogicalDevice.cxx
        src/vulkan/VulkanMemoryAllocator.cxx src/vulkan/SwapChain.cxx
        src/vulkan/ImageViews.cxx src/vulkan/RenderPass.cxx
        src/vulkan/DescriptorSet.cxx src/vulkan/Buffers.cxx
        src/vulkan/GraphicsPipeline.cxx src/vulkan/FrameBuffers.cxx
        src/vulkan/CommandPool.cxx src/vulkan/Synchronization.cxx
        src/vulkan/Vulkan.cxx include/vulkray.h)

# Engine binary target (.so/.dll/.dylib)
add_library(${name} SHARED ${sources})

# Setup linked libraries using Conan info
target_link_libraries(${name} ${CONAN_LIBS})

# GLSL shaders to Spir-V
add_subdirectory(shaders)
add_dependencies(${name} vulkray-shaders)

# Google Tests
enable_testing()
add_subdirectory(tests)
