cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/external)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui)
include_directories(${CMAKE_SOURCE_DIR}/external/imgui/backends)

if (WIN32)
    # Настройка для Windows
#    set(GLUT_LIBRARIES "C:/path_to_glut/freeglut/lib/freeglut.lib")  # Укажите правильный путь
#    include_directories("C:/path_to_glut/freeglut/include")           # Укажите путь к заголовочным файлам FreeGLUT

    # Если используете vcpkg, может выглядеть как:
    # set(GLUT_LIBRARIES "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/lib/freeglut.lib")
    # include_directories("${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows/include")

#elseif(APPLE)
#    # Настройки для macOS
#    find_package(GLUT REQUIRED)
#    include_directories(${GLUT_INCLUDE_DIRS})
#    set(GLUT_LIBRARIES "-framework GLUT -framework OpenGL")
#else()
#    # Настройки для Linux
#    find_package(GLUT REQUIRED)
#    include_directories(${GLUT_INCLUDE_DIRS})
endif()

add_executable(computer-graphics src/main.cpp
        src/includes.h
        src/utils/rgb_to_hsv.h
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
        src/utils/converters.h
        src/utils/draw_bresenham_line.h
        src/views/create_line_controls.h
        src/utils/handle_mouse_click_on_image.h
        src/views/create_tools.h
        src/utils/wu.h
        src/views/editor/editor.h
        src/models/Tool.h
        src/models/Line.h
        src/models/Polygon.h
        src/models/Point.h
        src/utils/check_point_position_relative_edge.h
        src/utils/Is_point_inside_polygon.h
        src/models/Color.h
)

#target_link_libraries(computer-graphics ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(computer-graphics OpenGL::GL glfw)
