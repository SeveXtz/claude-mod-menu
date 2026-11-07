cmake_minimum_required(VERSION 3.20)
project(ClaudeMenu CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Baixa o Dear ImGui automaticamente (nao precisa de git)
include(FetchContent)
FetchContent_Declare(imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.91.5.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(imgui)

add_executable(ClaudeMenu WIN32
    src/main.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx11.cpp)

target_include_directories(ClaudeMenu PRIVATE src ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(ClaudeMenu PRIVATE UNICODE _UNICODE NOMINMAX IMGUI_DEFINE_MATH_OPERATORS)
target_link_libraries(ClaudeMenu PRIVATE d3d11 dxgi dwmapi)

if(MSVC)
    target_compile_options(ClaudeMenu PRIVATE /utf-8)
endif()
