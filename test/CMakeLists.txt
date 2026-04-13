cmake_minimum_required(VERSION 3.20)
project(jsonfish_test LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} ./main.c ../jsonfish.c)

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 99
)
