# Build and Deployment Guide

## Prerequisites

- C++17 compatible compiler (e.g., GCC 9+, Clang, MSVC)
- CMake version 3.15 or higher
- Required external libraries:
  - GLFW
  - OpenGL
  - GLAD
  - glm
  - nlohmann_json (header-only)

## Building the Project

### 1. Clone the repository

```bash
git clone <repository-url>
cd adsil_analyzer_cpp
```

### 2. Create and enter build directory

```bash
mkdir build
cd build
```

### 3. Configure the build

```bash
cmake ..
```

Optionally specify build type:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### 4. Build the project

```bash
cmake --build .
```

Or with parallel jobs:

```bash
cmake --build . -- -j$(nproc)
```

### 5. Run tests

```bash
ctest
```

Or for verbose output:

```bash
ctest --verbose
```

---

## Deployment

- The final executables are located in `build/bin/` directory.
- The `resources/` folder is copied automatically to the binary directory during build.
- Make sure to package both the binary and the `resources/` folder when deploying.

---

## Notes

- Modify `CMakeLists.txt` files if you add new modules or dependencies.
- Use environment variables or CMake options for configurable paths.
- Keep external dependencies updated but pinned to tested versions for stability.

---

## Troubleshooting

- Missing dependencies: Ensure all required libraries are installed and found by CMake.
- Compilation errors: Verify compiler compatibility and flags.
- Runtime errors: Check resource file paths and environment setup.

---

Would you like me to help create CI/CD pipeline configs (GitHub Actions, GitLab CI, etc.) next?
