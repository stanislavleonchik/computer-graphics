cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/imgui)
include_directories(${CMAKE_SOURCE_DIR}/include/imgui/backends)

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

add_executable(computer-graphics main.cpp
        lab1/draw_graph.h
        includes.h
        lab2/rgb2hsv/rgb2hsv.h
        include/stb_image.h
        include/stb_image_write.h
        lab2/rgb2hsv/load_image.h
        lab2/rgb2hsv/create_sliders.h
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        lab2/rgb2hsv/converters.h
        lab3/task2/draw_bresenham_line.h
        lab3/task2/create_line_controls.h
        lab3/task2/handle_mouse_click_on_image.h
        lab3/task2/create_tools.h
        lab3/task2/wu.h
)

#target_link_libraries(computer-graphics ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(computer-graphics OpenGL::GL glfw)
