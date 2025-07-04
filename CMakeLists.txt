cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Add vendored dependencies
add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/fmt)

# Assimp local include + lib
set(ASSIMP_DIR "${CMAKE_SOURCE_DIR}/vendor/assimp")
set(ASSIMP_INCLUDE_DIR "${ASSIMP_DIR}/include")
set(ASSIMP_LIBRARY "${ASSIMP_DIR}/lib/libassimp.dylib")

# Set up include + link
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/renderer
    ${CMAKE_SOURCE_DIR}/include
    ${ASSIMP_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE glfw 
    PRIVATE glad 
    PRIVATE glm
    PRIVATE imgui
    PRIVATE fmt::fmt
    PRIVATE ${ASSIMP_LIBRARY}
)

# Copy runtime assets
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shader DESTINATION ${CMAKE_BINARY_DIR})
