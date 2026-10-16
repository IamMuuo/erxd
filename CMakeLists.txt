cmake_minimum_required(VERSION 3.25)

project(erxd LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

add_executable(erxd src/main.cpp)

target_compile_features(erxd PRIVATE cxx_std_20)
target_link_libraries(erxd PRIVATE Boost::system Threads::Threads)

if(MSVC)
  target_compile_options(erxd PRIVATE /W4 /permissive-)
else()
  target_compile_options(erxd PRIVATE -Wall -Wextra -Wpedantic)
endif()
