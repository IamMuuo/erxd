cmake_minimum_required(VERSION 3.25)

project(erxd LANGUAGES CXX)

add_executable(erxd src/main.cpp)

target_compile_features(erxd PRIVATE cxx_std_20)

if(MSVC)
  target_compile_options(erxd PRIVATE /W4 /permissive-)
else()
  target_compile_options(erxd PRIVATE -Wall -Wextra -Wpedantic)
endif()
