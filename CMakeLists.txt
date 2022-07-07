cmake_minimum_required(VERSION 3.23)
project(learning_vulkan)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lvulkan -lglfw")

add_executable(learning_vulkan src/initialize.cxx)
