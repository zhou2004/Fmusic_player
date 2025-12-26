
<div align=center>
<img src="./src/Assets/icon.svg" alt="FMusic logo" width="110" />

# FMusic

基于 Qt 的跨平台音乐播放器 🎵🎉

</div>

<div align=center>

[English](README.md) | 简体中文

[![Status](https://img.shields.io/badge/status-active%20development-yellowgreen)](https://github.com/zhou2004/Fmusic_player)
[![License](https://img.shields.io/badge/license-TBD-lightgrey)](#license)
[![Qt](https://img.shields.io/badge/Qt-6.8.1-41b883?logo=qt)](https://www.qt.io)
[![Language](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B)](https://isocpp.org)
[![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)](#)

FMusic 是一款跨平台音乐播放器，遵循 Fluent UI 设计理念。
用户界面采用 Qt QML 实现，核心功能使用现代 C++ 编写，
在条件允许的情况下支持 Windows、macOS、Linux、Android、iOS 以及 Web 平台。

</div>

> 注意：本项目处于积极开发阶段，许多功能仍在完善中。欢迎贡献代码——如果你希望参与开发，请联系作者。😉

## 背景 🧭

随着数字音乐的普及，用户更期待一款简单、快速、注重隐私、并能跨设备无缝使用的播放器。  
许多现有播放器存在平台受限、广告过多或功能复杂臃肿等问题。  
FMusic 希望成为一个轻量、视觉一致（Fluent UI）、开源且功能逐步完善的播放器，同时也是开发者的个人学习项目。

## 构建（Windows 10 / 11）🛠️

### 先决条件

1. C++ 工具链：Visual Studio 2022（安装“使用 C++ 的桌面开发”工作负载）或 MinGW  
2. CMake：3.20 或更高版本  
3. Conan：2.0+（包管理器）  
4. Qt：Qt 6.8.1（确保设置了 `QT_ROOT` 环境变量，或保证 CMake 能正确找到 Qt）

### 首次配置（Conan）

首次构建前，请为 Conan 检测并生成当前系统的编译器配置：

```bash
conan profile detect --force
````

### 命令行构建

克隆仓库：

```bash
git clone https://github.com/zhou2004/Fmusic_player.git
cd Fmusic_player
```

构建 Release 版本：

```bash
# 1. 配置（会自动通过 Conan 安装依赖）
cmake --preset FMusic-release

# 2. 构建
cmake --build --preset release
```

构建 Debug 版本：

```bash
# 1. 配置
cmake --preset FMusic-debug

# 2. 构建
cmake --build --preset debug
```

> 注意：可执行文件会生成在 `bin/Release` 或 `bin/Debug` 目录下。

### 使用 CLion 构建（推荐）

1. 使用 CLion 打开 `Fmusic_player` 目录
2. CLion 会自动识别 `CMakePresets.json`
3. 进入 `Settings -> Build, Execution, Deployment -> CMake`，启用 `FMusic-release` 或 `FMusic-debug` 配置
4. 等待首次配置完成（Conan 会自动下载依赖）
5. 点击 Build（小锤子）开始构建

## 核心功能 ✨

基础功能

- 🎧 本地播放：支持 MP3、WAV、FLAC 等常见格式
- ▶️ 播放控制：播放、暂停、拖动进度、上一首/下一首
- 🔊 音量控制
- 📝 歌词：支持本地歌词文件与在线同步
- 🌓 主题：支持浅色/深色模式（后续将持续改进）
- 🗂️ 播放列表管理：创建、编辑、删除播放列表（计划增强）

扩展 / 规划中

- 🎚️ 内置均衡器与音效
- 🔁 音频格式转换工具
- ☁️ 云端与在线音乐整合（后续工作）

## 约束与技术栈

- 语言：C++20（Qt 6.8.1）
- UI：QML（Fluent UI 风格）
- 编译器：MinGW64 或 MSVC（x64）
- 构建系统：CMake 3.20+（推荐 Ninja）

## 贡献指南 🤝

欢迎贡献代码、提交 Bug、提出建议。请在仓库中创建 Issue 或提交 Pull Request。
如果你希望直接加入开发，可以联系项目作者。

## 第三方库 📚

本项目受益于许多优秀的开源项目，主要依赖包括：

* **[Qt 6.8.1](https://www.qt.io)** - 强大的跨平台应用开发框架
* **[FluentUI for QML](https://github.com/zhuzichu520/FluentUI)** - 面向 Qt/QML 的 Fluent 设计组件库
* **[TagLib 2.0](https://taglib.org/)** - 音频元数据读写库（ID3、Vorbis 等）
* **[KissFFT 131.1.0](https://github.com/mborgerding/kissfft)** - FFT 库，用于音频频谱可视化
* **[OpenSSL 3.0.15](https://www.openssl.org/)** - TLS 安全通信工具套件
* **[zlib 1.3.1](https://zlib.net/)** - 轻量级压缩库

*依赖管理由 **[Conan](https://conan.io)** 统一完成。*

## License 📜

本项目使用 GPL 2.0 许可证开源发布，详见 LICENSE 文件。

