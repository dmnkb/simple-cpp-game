cmake_minimum_required(VERSION 3.20.0)

project(SimpleCPPGame)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)

add_executable(${PROJECT_NAME} ${MY_SOURCES})

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
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    # ${CMAKE_SOURCE_DIR}/src/scene
    ${CMAKE_SOURCE_DIR}/include  # Add the include directory
    # Add more directories if needed
)

file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})