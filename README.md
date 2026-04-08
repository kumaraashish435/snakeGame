# Snake Game

A production-ready Snake game built with **C++20** and **SDL3**, targeting
Windows, macOS, Linux, Android, and Web (Emscripten/WebAssembly).

## Folder Structure

```
snakeGame/
├── CMakeLists.txt
├── README.md
├── src/
│   ├── main.cpp          # Entry point (desktop + Emscripten)
│   ├── Config.h          # Compile-time constants
│   ├── Types.h           # Vec2i, Direction, GameState, Color, Theme
│   ├── Snake.h / .cpp    # Snake entity (body, movement, collision)
│   ├── Food.h  / .cpp    # Food spawning
│   ├── Audio.h / .cpp    # Procedural sound effects (SDL3 audio)
│   ├── Renderer.h / .cpp # SDL3 rendering + pixel-art bitmap font
│   └── Game.h / .cpp     # Game loop, input, state machine
└── web/
    └── shell.html        # Emscripten HTML shell template
```

## Features

| Feature                | Details                                         |
|------------------------|-------------------------------------------------|
| Controls               | Arrow keys, WASD, touch/swipe (mobile)          |
| Pause                  | `P` or `Escape`                                 |
| Theme toggle           | `T` switches dark / light theme                 |
| Restart                | `Space` or `Enter` (tap on mobile)              |
| Score & high score     | Pixel-art HUD; high score saved to disk         |
| Increasing difficulty  | Speed ramps from 8 → 20 moves/sec               |
| Smooth animation       | Fixed-timestep with interpolation at 60 FPS     |
| Procedural audio       | Rising chirp on eat, descending buzz on death   |
| Cross-platform         | Windows, macOS, Linux, Android, Web             |

## Build Instructions

### Prerequisites

- **CMake ≥ 3.20**
- **C++20 compiler** (GCC 11+, Clang 13+, MSVC 2022+)
- **SDL3** — automatically fetched via CMake FetchContent if not installed

### macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.ncpu)
./build/SnakeGame
```

### Linux

```bash
# Optional: install SDL3 system-wide, or let CMake fetch it
# sudo apt install libsdl3-dev   # if packaged

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/SnakeGame
```

### Windows (Visual Studio 2022)

```powershell
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
.\build\Release\SnakeGame.exe
```

Or with Ninja:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
.\build\SnakeGame.exe
```

### Android

Requires the Android NDK. Build the game as a shared library, then load it
from an SDL3 Android project.

```bash
cmake -S . -B build-android \
  -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-26 \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build-android -j$(nproc)
```

The resulting `libSnakeGame.so` must be placed inside an SDL3 Android project.
See the SDL3 documentation for the Java/Kotlin activity wrapper:
https://wiki.libsdl.org/SDL3/README/android

### Web (Emscripten / WebAssembly)

Requires the Emscripten SDK (`emsdk`).

```bash
source /path/to/emsdk/emsdk_env.sh

emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web -j$(nproc)
```

Outputs `build-web/SnakeGame.html`, `.js`, and `.wasm`.
Serve with any local HTTP server:

```bash
cd build-web
python3 -m http.server 8080
# Open http://localhost:8080/SnakeGame.html
```

## Controls Reference

| Input         | Action                     |
|---------------|----------------------------|
| Arrow keys    | Move snake                 |
| W / A / S / D | Move snake (alternative)   |
| P / Escape    | Pause / unpause            |
| T             | Toggle dark / light theme  |
| Space / Enter | Restart after game over    |
| Swipe         | Move snake (touch devices) |
| Tap           | Restart / unpause (touch)  |

## License

Public domain / MIT — use freely.
