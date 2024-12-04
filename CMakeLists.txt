cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)


find_package(OpenGL REQUIRED)

add_library(glad external/glad/src/glad.c)
include_directories(glad external/glad/include)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui/backends)
include_directories(${CMAKE_SOURCE_DIR}/src/views)
include_directories(${CMAKE_SOURCE_DIR}/src/models)
include_directories(${CMAKE_SOURCE_DIR}/src/utils)

add_executable(
		computer-graphics src/main.cpp
        external/imgui/imgui.h
		external/imgui/imgui.cpp
		external/imgui/imgui_draw.cpp
		external/imgui/imgui_widgets.cpp
		external/imgui/imgui_tables.cpp
		external/imgui/backends/imgui_impl_glfw.cpp
		external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_opengl3.h
        external/imgui/backends/imgui_impl_glfw.h
        external/stb/stb_image.h
        external/stb/stb_image_write.h

		src/models/Mesh.h
		src/models/Matrix4x4.h
		src/utils/load_obj.h
		src/utils/save_obj.h
)

target_link_libraries(computer-graphics OpenGL::GL glfw glad)
