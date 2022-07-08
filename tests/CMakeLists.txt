cmake_minimum_required(VERSION 3.23)
set(this UnitTests)

add_executable(${this} ExampleTests.cxx)
target_link_libraries(${this} PUBLIC gtest gtest_main)

add_test(NAME ${this} COMMAND ${this})
