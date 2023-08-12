空闲时间开发的一套引擎框架
* FlagGG - 引擎代码
* 3rd - 第三方库
* Application - App实例
* Samples - 一些测试样例
* Tools - 工具模块，非常不成熟，等基本盘搞定才会继续写

Visual Studio 2019 - 构建
```
# 构建动态库版本
./build_shared.bat

# 构建静态库版本
./build_static.bat
```

emscripten - 构建
```
# 如果第一次使用emscripten
./emscripten_init.bat

# 构建asm工程
./build_asm.bat
```

近期把引擎的图形层基本重构完毕了，大致上能用了，后面会持续优化
接下来改动方向：
* 管线脚本定制（目前图形管线已经重构成可以高度定制的管线了，这个应该会优先做一下）
* 支持多线程渲染
* 支持压缩纹理
* 内存管理
* 游戏运行时动态修改物件属性，热更新物件属性
* 增加OpenGL支持（这会优先做，主要是为了方便调试shader uniform）
* 增加metal支持
