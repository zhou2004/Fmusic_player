## 此项目正在开发中!!!
# 😶‍🌫️项目名称

基于qt的跨平台音乐播放器

# 💖项目背景

随着数字音乐的普及，用户对音乐播放器的需求日益增长。
现有的音乐播放器大多局限于单一平台，无法满足用户在不同设备上无缝切换的需求。
此外，许多播放器界面复杂，用户体验不佳，且存在广告过多、隐私保护不足等问题。
因此，开发一款跨平台、界面简洁、内存小功能强且开源的音乐播放器，增加自己学习计算机的能力，
而且具有重要的市场价值和社会意义

# 💕项目构建

### Windows 10/11 :
前置条件：
1. 安装 Qt 6.8.1 和 Qt Creator或者Clion。
2. mingw编译器11.0以上。
3. 安装 CMake 3.20 以上。
4. 生成器：Ninja或 make均可。
5. 安装 Vcpkg
6. 安装 MySQL 8.0 数据库。

克隆项目代码

```bash
git clone git@gitee.com:zhou2004jj/fmusic_player.git
```

进入项目目录

```bash
cd fmusic_player
```

安装依赖(1)
```bash
vcpkg install   #这个手动安装依赖为源码编译安装，时间较长,若失败或者网络问题可以使用下面的命令(2)
```

安装依赖(2) 

DCMAKE_MAKE_PROGRAM:生成器路径 , DCMAKE_C_COMPILER:C编译器路径 

DCMAKE_CXX_COMPILER:C++编译器路径 , DVCPKG_TARGET_TRIPLET:vcpkg构建平台目标(此处为mingw_x64) 

DCMAKE_TOOLCHAIN_FILE:vcpkg工具链文件路径 ,DVCPKG_INSTALLED_DIR:vcpkg依赖安装目录 , -G:生成器类型 -B :构建目录 -S:源代码目录

```bash
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_MAKE_PROGRAM=/path/to/ninja.exe -DCMAKE_C_COMPILER=/path/to/gcc.exe -DCMAKE_CXX_COMPILER=/path/to/g++.exe -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic -DCMAKE_TOOLCHAIN_FILE=D:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_INSTALLED_DIR=./vcpkg_installed -G Ninja -S . -B ./cmake-build-debug
```

构建编译项目

```bash
cmake --build ./cmake-build-debug --target all -j 10 #指定并行构建的线程数为10
```

# 🌟主要功能
## 基本功能
1. 本地音乐播放：支持 MP3、WAV、FLAC 等格式(√)。
2. 播放控制：播放、暂停、快进、快退、上一曲、下一曲(√)。
3. 音量控制：支持音量调节(√)。
4. 播放列表管理：创建、删除、编辑播放列表(待添加)。
5. 歌词显示：支持本地歌词加载和在线歌词同步(√)。
6. UI 主题切换：支持浅色、深色模式切换(待优化扩展)。
## 扩展功能
1. 音效增强：提供均衡器调节功能。
2. 格式转换：支持音频格式转换（可扩展）。
3. 在线音乐支持：支持云端音乐播放（未来扩展）。

# 需求优先级

| 需求 | 重要性 | 实现优先级 |
|:----|:----|:----|
| 本地音乐播放 | 高 | 第一阶段 |
| 播放控制（播放、暂停等） | 高 | 第一阶段 |
| 播放列表管理 | 高 | 第一阶段 |
| 音量控制 | 高 | 第一阶段 |
| 歌词显示 | 中 | 第二阶段 |
| UI 主题切换 | 中 | 第二阶段 |
| 音效调节 | 低 | 第二阶段 |
| 在线音乐支持 | 低 | 第三阶段 |

# 约束条件
1. 开发语言：C++（Qt6.8.1 框架）。
2. UI 框架：FluentUI（基于 QML）。
3. 数据库：MySQL 8.0。
4. 编译器：MinGW64 或 MSVC64。
5. 构建工具：CMake 3.20 以上。
6. 依赖管理：Vcpkg。




