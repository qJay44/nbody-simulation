include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW64_PATH}/gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW64_PATH}/g++.exe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MyProject VERSION 1.0)

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/run)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/pch.hpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${SFML_PATH}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENCL_PATH}/include)

target_link_directories(${PROJECT_NAME} PUBLIC ${SFML_PATH}/lib)
target_link_directories(${PROJECT_NAME} PUBLIC ${OPENCL_PATH}/lib/x86_64)
target_link_libraries(${PROJECT_NAME} sfml-system sfml-window sfml-graphics OpenCL)

