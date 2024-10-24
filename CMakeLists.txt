cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)
include_directories(external/glfw/include)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui/backends)

add_executable(
		computer-graphics src/main.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/backends/imgui_impl_opengl3.cpp
        external/imgui/backends/imgui_impl_glfw.cpp
        external/stb/stb_image.h
        external/stb/stb_image_write.h

        src/includes.h

		src/views/affine_tools.cpp
		src/views/editor/editor.cpp
		src/views/fractal_tools.cpp
		src/views/line_controls.cpp
		src/views/hsv_sliders.cpp
		src/views/drawing_tools_view.cpp
		src/views/midpoint_displacement_tools.cpp

		src/utils/converters/rgb_to_hsv_image_transform.cpp
		src/utils/converters/apply_hsv_adjustments.cpp
		src/utils/converters/hsv_to_rgb.cpp
		src/utils/converters/rgb_to_hsv.cpp
		src/utils/converters/image_to_texture.cpp
		src/utils/converters/update_texture.cpp

		src/supporting_files/imgui_support.cpp
        src/supporting_files/load_image.cpp
		src/supporting_files/handle_mouse_click_on_image.cpp

		src/models/Tool.cpp
		src/models/Line.cpp
		src/models/Point.cpp
		src/models/Polygon.cpp
		src/models/Color.cpp
		src/models/AffineMatrix.cpp
		src/models/MidpointDisplacementLine.cpp

		src/utils/polygon_transform/is_point_inside_polygon.cpp
		src/utils/polygon_transform/find_intersection.cpp
		src/utils/polygon_transform/check_point_position_relative_edge.cpp
		src/utils/midpoint_displacement/draw_midpoing_displacement.cpp
		src/utils/line_draw/draw_bresenham_line.cpp
		src/utils/line_draw/fill_all.cpp
		src/utils/line_draw/draw_wu_line.cpp
)

target_link_libraries(computer-graphics OpenGL::GL glfw)
