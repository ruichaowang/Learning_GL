# Learning_GL

- 在观察空间（而不是世界空间）中计算冯氏光照：参考解答 这个问题我不是特别理解
- view lookat 函数暂时没有深度研究，
- m1 和 x86 的 mac 都可以运行
- 相对地址总是有问题,用绝对地址就不会出问题
- [解决 mac 不能加载 stb image](https://github.com/Polytonic/Glitter/issues/70)

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