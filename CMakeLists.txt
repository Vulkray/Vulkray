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
set(sources
    src/Initialize.cxx
    src/vulkan/VulkanInstance.cxx src/vulkan/VulkanInstance.hxx
    src/vulkan/PhysicalDevice.cxx src/vulkan/PhysicalDevice.hxx
        src/vulkan/LogicalDevice.cxx src/vulkan/LogicalDevice.hxx src/vulkan/SurfaceKHR.cxx src/vulkan/SurfaceKHR.hxx)
# Executable targets
add_executable(${name} ${sources})

# Setup linked libraries using Conan info
target_link_libraries(${name} ${CONAN_LIBS})

# Google Tests
enable_testing()
add_subdirectory(tests)