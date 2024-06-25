cmake_minimum_required(VERSION 3.21)
project(open_gl_install)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_OSX_ARCHITECTURES  "arm64" CACHE STRING "Build architectures for Mac OS X" FORCE)

# 直接编译glfw
add_subdirectory(./include/glfw-3.4)

include_directories(./ ./include ./shaders ./include/glad ./include/KHR ./include/glm)

# glew 不支持 add_subdirectory，所以直接使用安装的 /opt/homebrew/Cellar/glew/2.2.0_1/include/GL  /opt/homebrew/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib
find_package(GLEW REQUIRED)

# 更换我们编译的 demo
# add_executable(OpenGL src/glad.c src/BowlModel.cpp src/occupancy_main.cpp)  //debug 确认障碍物生成是否准确
add_executable(OpenGL src/glad.c src/bowl_model_main.cpp src/BowlModel.cpp)
# add_executable(OpenGL src/glad.c src/voxol_main.cpp)  // voxel 的 SVM 效果

target_link_libraries(OpenGL glfw)
target_link_libraries(OpenGL GLEW::GLEW)


if (APPLE)
    target_link_libraries(OpenGL "-framework OpenGL")
    target_link_libraries(OpenGL "-framework GLUT")
endif ()