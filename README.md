空闲时间开发的一套引擎框架
* FlagGG - 引擎代码
* 3rd - 第三方库
* Application - App实例
* Samples - 一些测试样例
* Tools - 工具模块，非常不成熟，等基本盘搞定才会继续写

Visual Studio 2022 - 构建
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
* 管线脚本定制（目前图形管线已经重构成可以高度定制的管线了，这个优先级暂时不高，后面有时间再加）
* 支持多线程渲染
* 支持压缩纹理（完成）
* 内存管理（完成）
* 游戏运行时动态修改物件属性，热更新物件属性
* 增加OpenGL支持（这会优先做，主要是为了方便调试shader uniform）
* 增加metal支持（接了一版mtlpp，还有很多要改）
* 增加vulkan支持（基本完成，还有一些小问题）
* 多线程任务调度系统
 - ParallelTaskThreadPool，即时性高，线程任务不可被抛弃 （完成）
 - TaskThreadPool ，即时性稍低，线程任务可被抛弃（暂时用不到，等后面做大场景流式加载了在补全）
* 分层动画系统（目前完成基本盘，高阶混合树的方式之后再慢慢加上）
* 物理系统
* UI系统
