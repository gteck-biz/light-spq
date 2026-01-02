cmake_minimum_required(VERSION 4.1)
project(light)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)
add_executable(LightSPQ_run ${SOURCE_FILES})

include_directories(Light_SPQ)
add_subdirectory(Light_SPQ)

target_link_libraries(LightSPQ_run Light_SPQ)
add_subdirectory(Google_tests)