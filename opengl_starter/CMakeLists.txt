cmake_minimum_required (VERSION 3.8)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(lodepng CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")

add_executable (opengl_starter 
    "DebugDraw.cpp"
    "GltfLoader.cpp"
    "ImGuiHandler.cpp" 
    "opengl_starter.cpp" 
    "ParticleSystem.cpp"
    "TextRenderer.cpp"
    "Terrain.cpp"    
)

set_property(TARGET opengl_starter PROPERTY CXX_STANDARD 17)
# target_compile_options(opengl_starter PRIVATE /W4 /WX)

target_include_directories(opengl_starter PRIVATE
    ${CGLTF_INCLUDE_DIRS}
)

target_link_libraries(opengl_starter PRIVATE
    glad::glad
    glfw
    glm::glm
    imgui::imgui
    lodepng
    nlohmann_json 
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    spdlog::spdlog_header_only
)
