cmake_minimum_required(VERSION 3.23)
set(this Vulkan-Program)
project(${this})

# CMake Module Files
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

# Project dependencies
find_package(GLFW3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(GLM REQUIRED)
find_package(Git REQUIRED)
find_package(Ninja REQUIRED)
find_package(GTest REQUIRED)

# Using C++20 standard with g++ compiler
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER g++)
# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -DSPDLOG_FMT_EXTERNAL=1")

# Executable targets
add_executable(${this} src/Initialize.cxx src/vulkan/VulkanInstance.cxx src/vulkan/VulkanInstance.hxx)

# Linked dynamic libraries (SOs)
target_link_libraries(${this} glfw vulkan fmt)

# Unit testing
enable_testing()
add_subdirectory(tests)