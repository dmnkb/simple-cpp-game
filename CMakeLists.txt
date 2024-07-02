cmake_minimum_required(VERSION 3.20.0)

project(Hello)

set(CMAKE_CXX_STANDARD 23)

# if (CMAKE_BUILD_TYPE MATCHES Release)
#     set(CMAKE_CXX_FLAGS "-O3")
# elseif (CMAKE_BUILD_TYPE MATCHES Trace)
#     set(CMAKE_CXX_FLAGS "-O3")
#     add_compile_definitions(ENABLE_TRACING)
# elseif (CMAKE_BUILD_TYPE MATCHES Debug)
#     set(CMAKE_CXX_FLAGS "-g")
# endif ()

file(GLOB_RECURSE MY_SOURCES src/*.cpp include/*.h)
add_executable(${PROJECT_NAME} ${MY_SOURCES})


add_subdirectory(vendor/glfw EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glad EXCLUDE_FROM_ALL)
add_subdirectory(vendor/glm EXCLUDE_FROM_ALL)

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")

target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glfw glad)