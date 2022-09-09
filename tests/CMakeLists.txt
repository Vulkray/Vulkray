cmake_minimum_required(VERSION 3.23)
set(this UnitTests)

add_executable(${this} ExampleTests.cxx)
target_link_libraries(${this} PUBLIC gtest gtest_main)

# Compile example C++ program, linked with the engine shared library
add_executable(VulkrayExampleProgram TestProgram.cxx)
target_link_libraries(VulkrayExampleProgram PUBLIC vulkray)

add_test(NAME ${this} COMMAND ${this})