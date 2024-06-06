# Learning_GL

- main.cpp 是形成了箱子的地图，每一个箱子都贴图了，有环境光照，有鼠标/键盘
- voxol_main ，去除了环境光照，使用相机纹理，计划是加载6张纹理的，先尝试加载1张纹理，找到正确的纹理坐标

## 编译方法

- cmke 编译，可以直接使用 vscode 集成

```shell
cd ..
rm -rf build
mkdir -p build
cd build
cmake ..
make
./OpenGL
```

## env

- glfw 直接编译
- glew 使用了系统的库
- 下载 glad C++ opengl4.6 core  https://glad.dav1d.de/
- 为了方便，路径用绝对路径，/Users/wangruichao/Work/Learning_GL/

## 代码资源

- [Camera](https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_9)
- [Coordinate Systems](https://learnopengl-cn.github.io/01%20Getting%20started/08%20Coordinate%20Systems/)
- [Transformations](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
- [纹理](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
- [解决 mac 不能加载 stb image](https://github.com/Polytonic/Glitter/issues/70)
- [学习 shaders](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/)
- [第二章的 EBO 源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp)
- [第二章源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.1.hello_triangle/hello_triangle.cpp)
- [第二章说明](https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/)
- [第一章说明](https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/)
- [第一章源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.2.hello_window_clear/hello_window_clear.cpp)
- [光照基础](https://learnopengl-cn.readthedocs.io/zh/latest/02%20Lighting/02%20Basic%20Lighting/)
