include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
message(STATUS "Local .env.cmake: ${LOCAL_ENV}")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER ${MINGW64_PATH}/gcc.exe)
set(CMAKE_CXX_COMPILER ${MINGW64_PATH}/g++.exe)

set(CMAKE_CXX_STANDARD 17)
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

# NOTE: Compiling only if have Vulkan
#================= Build SHADERS =================#

# Find all vertex and fragment sources within shaders directory
# taken from VBlancos vulkan tutorial
# https://github.com/vblanco20-1/vulkan-guide/blob/all-chapters/CMakeLists.txt
find_program(GLSL_VALIDATOR glslangValidator HINTS
  ${VULKAN_GLSLANG_VALIDATOR_EXECUTABLE}
  /usr/bin
  /usr/local/bin
  ${VULKAN_SDK_PATH}/Bin
  ${VULKAN_SDK_PATH}/Bin32
  $ENV{VULKAN_SDK}/Bin/
  $ENV{VULKAN_SDK}/Bin32/
)

# get all .vert and .frag files in shaders directory
file(GLOB_RECURSE GLSL_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/src/shaders/*.frag"
  "${PROJECT_SOURCE_DIR}/src/shaders/*.vert"
)

message(STATUS "BUILDING SHADERS")
foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/${FILE_NAME}.spv")
  message(STATUS ${GLSL})
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)


