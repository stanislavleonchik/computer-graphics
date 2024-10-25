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

        src/includes.h

		src/views/affine_tools.h
		src/views/editor/editor.h
		src/views/fractal_tools.h
		src/views/line_controls.h
		src/views/hsv_sliders.h
		src/views/drawing_tools_view.h
		src/views/midpoint_displacement_tools.h

		src/utils/converters/rgb_to_hsv_image_transform.h
		src/utils/converters/apply_hsv_adjustments.h
		src/utils/converters/hsv_to_rgb.h
		src/utils/converters/rgb_to_hsv.h
		src/utils/converters/image_to_texture.h
		src/utils/converters/update_texture.h

		src/supporting_files/imgui_support.h
        src/supporting_files/load_image.h
		src/supporting_files/handle_mouse_click_on_image.h

		src/models/Tool.h
		src/models/Line.h
		src/models/Point.h
		src/models/Polygon.h
		src/models/Color.h
		src/models/AffineMatrix.h
		src/models/MidpointDisplacementLine.h

		src/utils/polygon_transform/is_point_inside_polygon.h
		src/utils/polygon_transform/find_intersection.h
		src/utils/polygon_transform/check_point_position_relative_edge.h
		src/utils/midpoint_displacement/draw_midpoing_displacement.h
		src/utils/line_draw/draw_bresenham_line.h
		src/utils/line_draw/fill_all.h
		src/utils/line_draw/draw_wu_line.h
		src/models/Tool.h
)

target_link_libraries(computer-graphics OpenGL::GL glfw)
