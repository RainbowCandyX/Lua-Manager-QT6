# Lua Manager QT6

基于 Qt6 和 LuaJIT 的 Lua 脚本管理器

## 项目简介

Lua Manager QT6 是一个使用 Qt6 框架开发的 Lua 脚本管理应用程序，集成了 LuaJIT 和 sol2 库，提供高性能的 Lua 脚本执行环境。

## 功能特性

- 基于 Qt6 的现代化图形界面
- 集成 LuaJIT 2.1 高性能 Lua 引擎
- 使用 sol2 库实现 C++ 与 Lua 的无缝交互
- 跨平台支持（macOS、Windows、Linux）

## 技术栈

- **Qt 6.5+**: 图形界面框架
- **LuaJIT 2.1**: 高性能 Lua JIT 编译器
- **sol2**: C++/Lua 绑定库
- **CMake**: 构建系统
- **C++17**: 编程语言标准

## 环境要求

### macOS
- macOS 11.0 或更高版本
- Qt 6.5 或更高版本
- Xcode Command Line Tools
- CMake 3.19 或更高版本

### Windows
- Windows 10 或更高版本
- Qt 6.5 或更高版本
- MSVC 2019 或更高版本
- CMake 3.19 或更高版本

### Linux
- Qt 6.5 或更高版本
- GCC 或 Clang 编译器
- CMake 3.19 或更高版本

## 构建指南

### macOS

1. 安装依赖：
```bash
# 如果使用 Homebrew 安装 Qt
brew install qt@6 cmake

# 或从 Qt 官网下载安装
# https://www.qt.io/download
```

2. 编译 LuaJIT：
```bash
cd LuaJIT-2.1
export MACOSX_DEPLOYMENT_TARGET=11.0
make
cd ..
```

3. 配置并构建项目：
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/macos ..
cmake --build .
```

### Windows

1. 编译 LuaJIT：
```batch
cd LuaJIT-2.1\src
msvcbuild.bat
cd ..\..
```

2. 使用 CMake 构建：
```batch
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64 ..
cmake --build . --config Release
```

### Linux

1. 编译 LuaJIT：
```bash
cd LuaJIT-2.1
make
cd ..
```

2. 构建项目：
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..
cmake --build .
```

## 项目结构

```
Lua-Manager-QT6/
├── LuaJIT-2.1/          # LuaJIT 源码
├── sol/                 # sol2 头文件库
├── lua/                 # Lua 脚本文件
├── CMakeLists.txt       # CMake 配置文件
├── main.cpp             # 程序入口
├── widget.cpp/h/ui      # Qt 主窗口
└── README.md            # 项目说明
```

## 使用说明

启动应用程序后，可以在界面中：
- 编写和执行 Lua 脚本
- 管理 Lua 脚本文件
- 查看脚本执行结果

## 开发计划

- [ ] 添加脚本编辑器语法高亮
- [ ] 实现脚本调试功能
- [ ] 支持脚本导入导出
- [ ] 添加更多 Lua 库支持

## 许可证

本项目使用的第三方库许可证：
- LuaJIT: MIT License
- Qt: LGPL/Commercial License
- sol2: MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！

## 联系方式

- GitHub: [RainbowCandyX](https://github.com/RainbowCandyX)
- 项目地址: [Lua-Manager-QT6](https://github.com/RainbowCandyX/Lua-Manager-QT6)
