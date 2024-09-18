cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

if(WIN32)
    set(GLUT_LIBRARIES "C:/MinGW/lib/libfreeglut.a")
    include_directories("C:/MinGW/include")
elseif(APPLE)
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
    set(GLUT_LIBRARIES "-framework GLUT -framework OpenGL")
    include_directories("${CMAKE_SOURCE_DIR}/include")
    include_directories(${CMAKE_SOURCE_DIR}/include/imgui)
    include_directories(${CMAKE_SOURCE_DIR}/include/imgui/backends)
    include_directories(/usr/local/include)
else()
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
endif()

add_executable(computer-graphics main.cpp
        draw_graph.h
        includes.h
        rgb2hsv/rgb2hsv.h
        include/stb_image.h
        include/stb_image_write.h
        rgb2hsv/load_image.h
        rgb2hsv/create_sliders.h
        include/imgui/imgui.cpp
        include/imgui/imgui_draw.cpp
        include/imgui/imgui_tables.cpp
        include/imgui/imgui_widgets.cpp
        include/imgui/backends/imgui_impl_opengl3.cpp
        include/imgui/backends/imgui_impl_glfw.cpp
        rgb2hsv/converters.h
)
target_link_libraries(computer-graphics ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
target_link_libraries(computer-graphics OpenGL::GL glfw)
