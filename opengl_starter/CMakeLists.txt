cmake_minimum_required (VERSION 3.8)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")

add_executable (opengl_starter "opengl_starter.cpp" "Mesh.h" "Texture.h" "Shader.h" "Window.h" "Common.h" "Framebuffer.h")

set_property(TARGET opengl_starter PROPERTY CXX_STANDARD 17)
# target_compile_options(opengl_starter PRIVATE /W4 /WX)

target_include_directories(opengl_starter PRIVATE
    ${CGLTF_INCLUDE_DIRS}
)

target_link_libraries(opengl_starter PRIVATE
    glad::glad
    glfw
    glm::glm
    lodepng
    spdlog::spdlog 
    spdlog::spdlog_header_only
)
