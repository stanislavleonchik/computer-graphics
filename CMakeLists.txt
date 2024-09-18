cmake_minimum_required(VERSION 3.28)
project(computer-graphics)

set(CMAKE_CXX_STANDARD 26)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

if(WIN32)
    set(GLUT_LIBRARIES "C:/MinGW/lib/libfreeglut.a")
    include_directories("C:/MinGW/include")
elseif(APPLE)
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
    set(GLUT_LIBRARIES "-framework GLUT -framework OpenGL")
else()
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIRS})
endif()

add_executable(computer-graphics main.cpp)
target_link_libraries(computer-graphics ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
