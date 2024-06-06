# Learning_GL

- 当前环境是直接依赖于 mac 里的glfw环境，所以有依赖问题，安装 brew install glfw ，如果版本不对记得在 cmakelist 里修改 版本（也许是版本不一致）， 当前的依赖化境是直接从 homebrew 中读取的，如果可以应该直接去拿到工程中来
- main.cpp 是形成了箱子的地图，每一个箱子都贴图了，有环境光照，有鼠标/键盘

## 编译方法
```
cd ..
rm -rf build
mkdir -p build
cd build
cmake ..
make
./OpenGL
```

## env
```
brew install glfw

#为了方便，路径用绝对路径
/Users/wangruichao/Work/Learning_GL/
```

## 其他
- m1 和 x86 的 mac 都可以运行
-
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
- [光照基础](https://learnopengl-cn.readthedocs.io/zh/latest/02%20Lighting/02%20Basic%20Lighting/)

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