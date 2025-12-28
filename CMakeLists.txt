cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Enable precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

# Add vendored dependencies
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/fmt)
add_subdirectory(vendor/entt)
add_subdirectory(vendor/imgui)

# yaml-cpp (disable extras)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
# Some versions use these:
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

# Assimp local include + lib
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/vendor/assimp")
set(ASSIMP_INCLUDE_DIR "${ASSIMP_DIR}/include")
set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/libassimp.dylib")

# Set up include + link
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${ASSIMP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/vendor/stduuid/include
    ${CMAKE_SOURCE_DIR}/vendor/entt/src
    ${CMAKE_SOURCE_DIR}/vendor/yaml-cpp/include/
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE glfw 
    PRIVATE glad 
    PRIVATE glm
    PRIVATE imgui
    PRIVATE imgui_impl_glfw
    PRIVATE imgui_impl_opengl3
    PRIVATE fmt::fmt
    PRIVATE ${ASSIMP_LIBRARY}
    PRIVATE yaml-cpp
)

# Copy runtime assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shader DESTINATION ${CMAKE_BINARY_DIR})
