
<div align=center>
<img src="./src/Assets/icon.svg" alt="FMusic logo" width="110" />

# FMusic

Qt-based Cross-Platform Music Player 🎵🎉

</div>

<div align=center>

English | [简体中文](README_zh_CN.md)

[![Windows](https://github.com/zhou2004/Fmusic_player/actions/workflows/build_msix.yml/badge.svg)](https://github.com/zhou2004/Fmusic_player/actions/workflows/build_msix.yml)
[![License](https://img.shields.io/badge/license-GPL-lightgrey)](./LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.8.1-41b883?logo=qt)](https://www.qt.io)
[![Language](https://img.shields.io/badge/language-C%2B%2B-blue?logo=c%2B%2B)](https://isocpp.org)
[![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)](#)

FMusic is a cross-platform music player that follows Fluent UI design principles. 
The user interface is implemented with Qt QML while core functionality is written in modern C++, 
enabling support for Windows, macOS, Linux, Android, iOS and web platforms where applicable.

</div>

> Note: This project is under active development. Many features are work-in-progress. Contributions are welcome — please contact the author if you would like to help. 😉

## Background 🧭

As digital music becomes ubiquitous, users expect a simple, 
fast and privacy-respecting player that works seamlessly across devices. 
Many existing players are platform-limited, ad-heavy, or overly complex. 
FMusic aims to be lightweight, visually consistent (Fluent UI), open-source, 
and feature-rich while serving as a personal learning project for the developer.

## Build (Windows 10 / 11) 🛠️

### Prerequisites: 

1. C++ Tools: Visual Studio 2022 (with C++ desktop development workload) or MinGW. 
2. CMake: Version 3.20 or newer. 
3. Conan: Version 2.0+ (Package Manager). 
4. Qt: Qt 6.8.1 (Ensure QT_ROOT environment variable is set or CMake can find it).

### First Time Setup (Conan):

Before building for the first time, detect your system's compiler profile for Conan:

```bash
conan profile detect --force
```

### Build via Command Line

Clone the repository:

```Bash
git clone https://github.com/zhou2004/Fmusic_player.git
cd Fmusic_player

```
Build Release Version:

```bash
# 1. Configure (This automatically installs dependencies via Conan)
cmake --preset FMusic-release

# 2. Build
cmake --build --preset release
```

Build Debug Version:

```bash
# 1. Configure
cmake --preset FMusic-debug

# 2. Build
cmake --build --preset debug
```

>Note: The executable will be generated in bin/Release or bin/Debug.

Build via CLion (Recommended)

1. Open the Fmusic_player folder in CLion. 
2. CLion will automatically detect CMakePresets.json. 
3. Go to Settings -> Build, Execution, Deployment -> CMake and enable the FMusic-release or FMusic-debug profile. 
4. Wait for the initial configuration to finish (Conan will download dependencies automatically). 
5. Click the Build (Hammer) icon.

## Key Features ✨

Basic
- 🎧 Local playback: MP3, WAV, FLAC, and other common formats.
- ▶️ Playback controls: play, pause, seek, previous/next track.
- 🔊 Volume control.
- 📝 Lyrics: support for local lyric files and online synchronization.
- 🌓 Theme support: light and dark modes (improvements planned).
- 🗂️ Playlist management: create, edit and remove playlists (enhancements planned).

Extended / Planned
- 🎚️ Built-in equalizer and audio effects.
- 🔁 Audio format conversion tools.
- ☁️ Cloud and online music integration (future work).

## Constraints & Technology Stack
- Language: C++ 20 (Qt 6.8.1)
- UI: QML with Fluent UI styling
- Compilers: MinGW64 or MSVC (x64)
- Build system: CMake 3.20+, Ninja recommended

## Contributing 🤝
Contributions, bug reports and suggestions are welcome. Please open issues or pull requests on the repository. 
If you want to join development directly, contact the project author.

## Third-Party Libraries 📚

This project stands on the shoulders of giants. We utilize the following open-source libraries to deliver the best experience:

- **[Qt 6.8.1](https://www.qt.io)** - Powerful cross-platform application development framework.
- **[FluentUI for QML](https://github.com/zhuzichu520/FluentUI)** - A fluent design system component library for Qt/QML.
- **[TagLib 2.0](https://taglib.org/)** - A library for reading and editing the meta-data of audio formats (ID3, Vorbis, etc.).
- **[KissFFT 131.1.0](https://github.com/mborgerding/kissfft)** - A Fast Fourier Transform (FFT) library, used for audio spectrum visualization.
- **[OpenSSL 3.0.15](https://www.openssl.org/)** - Robust, commercial-grade toolkit for the Transport Layer Security (TLS) protocol.
- **[zlib 1.3.1](https://zlib.net/)** - A massively spiffy yet delicately unobtrusive compression library.

*Dependency management is handled by **[Conan](https://conan.io)**.*

## License 📜

This project is licensed under the GPL License - see the [LICENSE](./LICENSE) file for details.
