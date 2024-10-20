cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui/backends)

add_executable(computer-graphics src/main.cpp
        src/includes.h
		src/utils/converters/rgb_to_hsv.h
        external/stb/stb_image.h
        external/stb/stb_image_write.h
        src/utils/load_image.h
        src/views/create_sliders.h
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
		src/utils/converters/converters.h
		src/utils/line_draw/draw_bresenham_line.h
        src/views/create_line_controls.h
        src/utils/handle_mouse_click_on_image.h
        src/views/create_tools.h
		src/utils/line_draw/wu.h
        src/views/editor/editor.h
        src/models/Tool.h
        src/models/Line.h
        src/models/Polygon.h
        src/models/Point.h
		src/utils/polygon_transform/check_point_position_relative_edge.h
		src/utils/polygon_transform/Is_point_inside_polygon.h
        src/models/Color.h
)

target_link_libraries(computer-graphics OpenGL::GL glfw)
