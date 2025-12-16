add_library(imgui 
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC .)

add_library(imgui_impl_glfw backends/imgui_impl_glfw.cpp)
target_include_directories(imgui_impl_glfw PUBLIC backends)
target_link_libraries(imgui_impl_glfw PUBLIC imgui glfw)

add_library(imgui_impl_opengl3 backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui_impl_opengl3 PUBLIC backends)
target_link_libraries(imgui_impl_opengl3 PUBLIC imgui glad)
