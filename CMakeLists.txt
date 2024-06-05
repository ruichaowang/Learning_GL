cmake_minimum_required(VERSION 3.21)
project(open_gl_install)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_OSX_ARCHITECTURES  "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

set(LOCAL_H /opt/homebrew/include)   # 加载系统库
include_directories(${LOCAL_H})

include_directories(./ ./include ./shaders ./include/glad ./include/KHR ./include/glm ./include/cnpy)

set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.4/include/GLFW)
link_directories(${GLEW_H} ${GLFW_H})

set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.4/lib/libglfw.3.dylib)
set(cnpy_lib ${CMAKE_SOURCE_DIR}/include/cnpy/lib/libcnpy.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK} ${cnpy_lib})

add_executable(OpenGL src/glad.c src/main.cpp)
# add_executable(OpenGL src/glad.c src/voxol_main.cpp)

if (APPLE)
    target_link_libraries(OpenGL "-framework OpenGL")
    target_link_libraries(OpenGL "-framework GLUT")
endif ()