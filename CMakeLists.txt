cmake_minimum_required(VERSION 3.25)

project(erxd LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Threads REQUIRED)

include(CTest)

if(BUILD_TESTING)
  include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.8.1
    GIT_SHALLOW TRUE
  )
  FetchContent_MakeAvailable(Catch2)
endif()

add_executable(erxd src/main.cpp src/backend_pool.cpp)

target_compile_features(erxd PRIVATE cxx_std_20)
target_include_directories(erxd PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(erxd PRIVATE Boost::system Threads::Threads)

if(MSVC)
  target_compile_options(erxd PRIVATE /W4 /permissive-)
else()
  target_compile_options(erxd PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
  add_executable(erxd_tests tests/main_test.cpp tests/backend_pool_test.cpp src/backend_pool.cpp)
  target_compile_features(erxd_tests PRIVATE cxx_std_20)
  target_include_directories(erxd_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
  target_link_libraries(erxd_tests PRIVATE Catch2::Catch2WithMain Boost::system Threads::Threads)

  include(Catch)
  catch_discover_tests(erxd_tests)
endif()
