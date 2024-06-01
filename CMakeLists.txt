include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW32_PATH}gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW32_PATH}g++.exe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.h)

target_include_directories(${PROJECT_NAME} PUBLIC ${SFML_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENCL_PATH}/include)

target_link_directories(${PROJECT_NAME} PUBLIC ${SFML_PATH}/lib)
target_link_directories(${PROJECT_NAME} PUBLIC ${OPENCL_PATH}/lib/x86_64)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics OpenCL)

