空闲时间开发的一套引擎框架
* FlagGG - 引擎代码
* 3rd - 第三方库
* Application - App实例
* Samples - 一些测试样例
* Tools - 工具模块，非常不成熟，等基本盘搞定才会继续写

执行准备工作
```
Windows平台执行：prepare.bat
其它平台执行：prepare.sh
```

Visual Studio 2022 - 构建
```
# 生成vs工程（动态库版本）
./generate_project_windows_shared.bat

# 生成vs工程（静态库版本）
./generate_project_windows_static.bat
```

MacOS - 构建
```
1.使用vscode打开目录
2.下载插件：CMake、CMakeTools
3.下载完后，侧边会出现CMake构建工具的叶签，点击按钮切换到CMake构建的界面
4.点击Configure All Projects，这一步这行CMake生成平台构建工程
5.在PROJECT OUTLINE树形界面选择需要构建的库，然后点击Build按钮进行编译
6.在Run And Debug界面选需要启动的二进制文件运行
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
* 支持多线程渲染（懒得写，有时间再搓吧）
* 支持压缩纹理（完成）
* 内存管理（完成）
* 游戏运行时动态修改物件属性，热更新物件属性
* 增加OpenGL支持（应该不会继续接入了，除非有比较大的空闲）
* 增加metal支持（接了一版mtlpp，还有很多要改）
* 增加vulkan支持（大部分功能可以用了，直接渲染到backbuffer有一些问题待解决）
* 多线程任务调度系统
    * ParallelTaskThreadPool，即时性高，线程任务不可被抛弃 （完成）
    * TaskThreadPool ，即时性稍低，线程任务可被抛弃（暂时用不到，等后面做大场景流式加载了再补全）
* 分层动画系统（基础版可以使用了，高阶混合树的方式之后再慢慢加上）
* 物理系统
* UI系统（大体上可以使用了，字体还存在一些缺陷）
    * 搭建了一个基础的UI渲染系统（完成）
    * 在渲染系统基础上接入了Ultralight库（完成）
        * UltralightCore.dll没有开源，因此我重写了这个动态库，使用自己引擎做了渲染的工作
        * 因为重写了渲染层，所以顺便就支持了webui和引擎ui相互嵌套的功能
* 渲染层功能
    * ClusterLighting（完成）
    * Hi-Z Culling（完成）
    * SSAO（完成）
        * NV的HBAO+
        * 混合形SSAO（MSSAO和HBAO的混合版本，参照UE5）
    * 体积雾
    * 体积光
    * SSR（ing）
    * TAA
    * GPU-Instance (完成)
    * GPU-Driven
* 烘焙系统（这个会作为一个长线任务，之后慢慢补全不同的烘焙系统）
