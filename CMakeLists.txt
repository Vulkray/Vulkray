cmake_minimum_required(VERSION 3.23)
project(learning_vulkan)

# Using C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
# NOTE: Use stdlib 'libc++' for latest implemented C++20 features
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -lglfw -lvulkan -lfmt -DSPDLOG_FMT_EXTERNAL=1")

add_executable(vulkan-program src/Initialize.cxx)
