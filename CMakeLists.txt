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

		src/views/affine_tools.h
		src/views/editor/editor.h
		src/views/fractal_view/fractal_tools.h
		src/views/line_controls.h
		src/views/hsv_sliders.h
		src/views/drawing_tools_view.h
		src/views/midpoint_displacement_tools.h

		src/utils/converters/update_texture.h

		src/supporting_files/imgui_support.h
		src/supporting_files/handle_mouse_click_on_image.h

		src/models/Tool.h
		src/models/Line.h
		src/models/Point.h
		src/models/Polygon.h
		src/models/AffineMatrix.h
		src/models/MidpointDisplacementLine.h

		src/utils/polygon_transform/is_point_inside_polygon.h
		src/utils/polygon_transform/find_intersection.h
		src/utils/polygon_transform/check_point_position_relative_edge.h
		src/utils/midpoint_displacement/draw_midpoing_displacement.h
		src/utils/line_draw/draw_bresenham_line.h
		src/utils/line_draw/draw_wu_line.h
		src/models/Tool.h
		src/models/Fractal/FractalParams.h
		src/models/Fractal/Fractal.h
		src/models/Fractal/Fractal.h
		src/models/Fractal/FractalVertex.h
		src/utils/fractals/fractal_utils.h
		src/models/Fractal/LSystem.h
		src/views/fractal_view/fractal_editor.h
		src/models/Fractal/TurtleState.h
)

target_link_libraries(computer-graphics OpenGL::GL glfw)
