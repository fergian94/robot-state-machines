cmake_minimum_required(VERSION 3.3)
project(state_machines)

# ~~~~~~~~~~~~~~~~~~~~~~ CMAKE SETTINGS ~~~~~~~~~~~~~~~~~~~~~~
add_compile_options(-std=c++17)

if(APPLE)
  set(CMAKE_C_COMPILER "/usr/bin/gcc")
  set(CMAKE_CXX_COMPILER "/usr/bin/g++")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
  message("compiling in debug mode")
  add_compile_definitions(DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG")

set(SOURCE_DIR ${CMAKE_HOME_DIRECTORY}/src)
set(DEPS_DIR ${CMAKE_HOME_DIRECTORY}/deps)

file(GLOB LIB_SOURCES ${SOURCE_DIR}/*.cpp)

include_directories(${DEPS_DIR}/tinyfsm/include
                    ${SOURCE_DIR}/state_machines)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})
target_link_libraries(${PROJECT_NAME} m)