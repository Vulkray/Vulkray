cmake_minimum_required(VERSION 3.22)
set(this SampleProgram)

# Compile example C++ program, linked with the engine shared library
add_executable(${this} TestProgram.cxx)
target_link_libraries(${this} PUBLIC vulkray)

add_test(NAME ${this} COMMAND ${this})
