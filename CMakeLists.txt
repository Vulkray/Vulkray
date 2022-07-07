cmake_minimum_required(VERSION 3.23)
project(learning_vulkan)

# Using C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
# NOTE: Use stdlib 'libc++' for latest implemented C++20 features
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++ -DLIBCXX_ENABLE_INCOMPLETE_FEATURES=ON")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSPDLOG_FMT_EXTERNAL=1")

add_executable(vulkan-program src/Initialize.cxx)
