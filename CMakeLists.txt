include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW32_PATH}gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW32_PATH}g++.exe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

find_package(SFML 2.6.0 COMPONENTS system window graphics REQUIRED)

include_directories(${MY_UTILS_PATH})

add_executable(MyProject
  ${SRC_DIR}engine/utils/ThreadPool.cpp
  ${SRC_DIR}main.cpp
  ${SRC_DIR}engine/opencl-bruteforce/RuntimeOpenCL.cpp
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${SRC_DIR}pch.h)

target_include_directories(MyProject PUBLIC ${OPENCL_PATH}/include)
target_link_directories(MyProject PUBLIC ${OPENCL_PATH}/lib/x86)
target_link_libraries(MyProject sfml-system sfml-window sfml-graphics OpenCL)


