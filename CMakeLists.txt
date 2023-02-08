cmake_minimum_required(VERSION 3.21)
project(open_gl_install)

set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_OSX_ARCHITECTURES  "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

# /usr/local/include
# set(LOCAL_H /usr/local/include)      #x86mac
set(LOCAL_H /opt/homebrew/include)   # 加载系统库
include_directories(${LOCAL_H})

include_directories(./ ./include ./shaders ./include/glad ./include/KHR ./include/glm) # 需要加载的库

# x86mac 用上面的， m1用下面的
# set(GLEW_H /usr/local/Cellar/glew/2.2.0_1/include/GL)
# set(GLFW_H /usr/local/Cellar/glfw/3.3.7/include/GLFW)
set(GLEW_H /opt/homebrew/Cellar/glew/2.2.0_1/include/GL)
set(GLFW_H /opt/homebrew/Cellar/glfw/3.3.7/include/GLFW)
link_directories(${GLEW_H} ${GLFW_H})


# set(GLEW_LINK /usr/local/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
# set(GLFW_LINK /usr/local/Cellar/glfw/3.3.7/lib/libglfw.3.3.dylib)
set(GLEW_LINK /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib)
set(GLFW_LINK /opt/homebrew/Cellar/glfw/3.3.7/lib/libglfw.3.dylib)
link_libraries(${OPENGL} ${GLEW_LINK} ${GLFW_LINK})

add_executable(OpenGL src/glad.c src/main.cpp)

if (APPLE)
    target_link_libraries(OpenGL "-framework OpenGL")
    target_link_libraries(OpenGL "-framework GLUT")
endif ()