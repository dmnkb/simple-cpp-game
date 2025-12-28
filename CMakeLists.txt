cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    include/*.h
)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Enable precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.h)

# -----------------------
# Vendored dependencies
# -----------------------
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/fmt EXCLUDE_FROM_ALL)
add_subdirectory(vendor/entt EXCLUDE_FROM_ALL)
add_subdirectory(vendor/imgui EXCLUDE_FROM_ALL)

# yaml-cpp (disable extras)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
# Some versions use these:
set(YAML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

# -----------------------
# Assimp (local dylib)
# -----------------------
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/vendor/assimp")
set(ASSIMP_INCLUDE_DIR "${ASSIMP_DIR}/include")
set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/libassimp.dylib")

# -----------------------
# Includes
# -----------------------
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/include
    ${ASSIMP_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/vendor/stduuid/include
)

# -----------------------
# Links
# -----------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    glad
    glm
    imgui
    imgui_impl_glfw
    imgui_impl_opengl3
    fmt::fmt
    EnTT::EnTT
    yaml-cpp
    ${ASSIMP_LIBRARY}
)

# -----------------------
# Copy runtime assets
# -----------------------
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shader DESTINATION ${CMAKE_BINARY_DIR})
