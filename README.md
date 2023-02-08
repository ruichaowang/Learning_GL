# Learning_GL
简单的 opengl 环境依赖 demo， 里面应该运行后是一个可以跟随鼠标运动的箱子贴图

## 使用方法
- 安装 glfw ，

```
mkdir -p build
cmake ..
make
./OpenGL
```

## 其他
- m1 和 x86 的 mac 都可以运行
- 相对地址总是有问题,用绝对地址就不会出问题
- [解决 mac 不能加载 stb image](https://github.com/Polytonic/Glitter/issues/70)
- 找不到 glad.h 文件

## 代码资源
- [Camera](https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/#_9)
- [Coordinate Systems](https://learnopengl-cn.github.io/01%20Getting%20started/08%20Coordinate%20Systems/)
- [Transformations](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
- [纹理](https://learnopengl-cn.github.io/01%20Getting%20started/06%20Textures/)
- [学习 shaders ](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/)
- [第二章的 EBO 源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.2.hello_triangle_indexed/hello_triangle_indexed.cpp)
- [第二章源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/2.1.hello_triangle/hello_triangle.cpp)
- [第二章说明](https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/)
- [第一章说明](https://learnopengl-cn.github.io/01%20Getting%20started/03%20Hello%20Window/)
- [第一章源代码](https://learnopengl.com/code_viewer_gh.php?code=src/1.getting_started/1.2.hello_window_clear/hello_window_clear.cpp)

## github 登录不上
- https://juejin.cn/post/6844904193170341896
```
sudo gedit /etc/hosts
140.82.113.4 github.com
sudo killall -HUP mDNSResponder;say DNS cache has been flushed
ping www.github.com
```

### 下载 glad
C++ opengl4.6 core  https://glad.dav1d.de/