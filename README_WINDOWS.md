# ADSIL Analyzer - Windows Build Guide

## 🛠️ Prerequisites

1. **Visual Studio 2019 or later** with C++ workload
2. **CMake 3.15+** - Download from [cmake.org](https://cmake.org/download/) and add to PATH
3. **Git** - Download from [git-scm.com](https://git-scm.com/download/win)

## 📦 Setup Dependencies (vcpkg - Recommended)

```cmd
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install required packages
vcpkg install glfw3:x64-windows glm:x64-windows
```

## 🏗️ Build Instructions

```cmd
# Clone and build
git clone https://github.com/yourusername/adsil_analyzer.git
cd adsil_analyzer
mkdir build && cd build

# Configure with vcpkg
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release

# Run
bin\Release\adsil_analyzer.exe
```

## 🔧 Troubleshooting

### "Could not find GLFW3" or "Could not find glm" errors

**Fix 1: Set paths manually**

```cmd
cmake .. -DGLFW3_ROOT="C:\path\to\glfw" -Dglm_ROOT="C:\path\to\glm"
```

**Fix 2: Environment variables**

```cmd
set GLFW3_ROOT=C:\path\to\glfw
set GLM_ROOT=C:\path\to\glm
cmake ..
```

### DLL errors at runtime

```cmd
# Copy Visual C++ Redistributables to executable directory
# Download from Microsoft website
```

### MSVC runtime conflicts

Add to your CMakeLists.txt:

```cmake
if(MSVC)
    set_property(TARGET ${PROJECT_NAME} PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()
```

## 🚀 Quick Build Script

Create `build_windows.bat`:

```batch
@echo off
if not exist "build" mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[your vcpkg path]/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
echo Build complete! Run: bin\Release\adsil_analyzer.exe
pause
```

## 📋 Dependencies Summary

| Library                    | Purpose           | Status                      |
| -------------------------- | ----------------- | --------------------------- |
| GLFW                       | Window management | Install via vcpkg           |
| GLM                        | Math library      | Install via vcpkg           |
| OpenGL                     | Graphics          | Built-in (graphics drivers) |
| GLAD, ImGui, nlohmann_json | Various           | Included in project         |

---

_For issues, verify all dependencies are installed and check CMake output for missing packages._
