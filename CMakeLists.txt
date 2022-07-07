cmake_minimum_required(VERSION 3.23)
project(learning_vulkan)

# Using C++20 standard with g++ compiler
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -DSPDLOG_FMT_EXTERNAL=1")
# Set linker included libraries
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglfw -lvulkan -lfmt")

add_executable(vulkan-program src/Initialize.cxx)
