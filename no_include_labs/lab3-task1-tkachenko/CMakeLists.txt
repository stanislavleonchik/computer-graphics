cmake_minimum_required(VERSION 3.29)
project(comp_graph_3)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/imgui)
include_directories(${CMAKE_SOURCE_DIR}/include/imgui/backends)

# Настройки для Windows с использованием vcpkg
# Убедитесь, что vcpkg интегрирован с CMake
set(VCPKG_ROOT "C:/Users/Ly4aznik/.vcpkg-clion/vcpkg")  # Укажите путь к вашему vcpkg
set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")


add_executable(comp_graph_3 main.cpp
        include/stb_image.h
        include/stb_image_write.h
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
)

target_link_libraries(comp_graph_3 OpenGL::GL glfw)
