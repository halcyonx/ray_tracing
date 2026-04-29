cmake_minimum_required(VERSION 3.16)
project(RayTracing CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_executable(RayTracing ${SOURCES} ${HEADERS})
target_include_directories(RayTracing PRIVATE src)

set_target_properties(RayTracing PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Mirror the swapped Debug/Release behaviour from premake5.lua:
# Release → debug symbols + DEBUG defined; Debug → optimized + NDEBUG defined.
target_compile_definitions(RayTracing PRIVATE
    $<$<CONFIG:Release>:DEBUG>
    $<$<CONFIG:Debug>:NDEBUG>
)
target_compile_options(RayTracing PRIVATE
    $<$<CONFIG:Release>:-g>
    $<$<CONFIG:Debug>:-O2>
)
