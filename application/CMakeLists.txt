cmake_minimum_required(VERSION 3.12.0)
project(Medvision VERSION 0.1.0)

add_subdirectory(dependencies/glfw EXCLUDE_FROM_ALL)
add_subdirectory(dependencies/glm EXCLUDE_FROM_ALL)

# Add the executable
add_executable(Medvision
    src/glad.c
    src/debug/debug.cpp
    src/vendor/stb_image/stb_image.cpp
    src/controller/app.cpp
    src/factory/factory.cpp

    src/renderer/shaders/shader.cpp
    src/renderer/texture/texture.cpp
    src/renderer/buffer/VertexArray.cpp
    src/renderer/buffer/VertexBuffer.cpp
    src/renderer/buffer/IndexBuffer.cpp

    src/vendor/imgui/imgui_demo.cpp
    src/vendor/imgui/imgui_draw.cpp
    src/vendor/imgui/imgui_impl_glfw.cpp
    src/vendor/imgui/imgui_impl_opengl3.cpp
    src/vendor/imgui/imgui_widgets.cpp
    src/vendor/imgui/imgui_tables.cpp
    src/vendor/imgui/imgui.cpp

    src/systems/render_system.cpp

    src/main.cpp
)

target_include_directories(Medvision PRIVATE dependencies)
target_include_directories(Medvision PRIVATE src/vendor)

target_link_libraries(Medvision glfw)
target_link_libraries(Medvision glm)