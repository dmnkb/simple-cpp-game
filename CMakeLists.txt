cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

# Find Assimp installed via Homebrew
find_package(Assimp REQUIRED)

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)
add_subdirectory(vendor/imgui)
add_subdirectory(vendor/fmt)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE glfw 
    PRIVATE glad 
    PRIVATE glm
    PRIVATE imgui
    PRIVATE fmt::fmt
    PRIVATE Assimp
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/renderer
    ${CMAKE_SOURCE_DIR}/include
    /opt/homebrew/include
)

target_link_directories(${PROJECT_NAME} PRIVATE /opt/homebrew/lib)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY shader DESTINATION ${CMAKE_BINARY_DIR})
