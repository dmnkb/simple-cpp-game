cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)

target_link_libraries("${CMAKE_PROJECT_NAME}" 
    PRIVATE glfw 
    PRIVATE glad 
    PRIVATE glm
)

target_include_directories("${CMAKE_PROJECT_NAME}" 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})