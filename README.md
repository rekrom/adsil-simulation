# ADSIL Analyzer

[![codecov](https://codecov.io/gh/rekrom/adsil-simulation/graph/badge.svg?token=UNQJ11QVP1)](https://codecov.io/gh/rekrom/adsil-simulation)
[![Build Status](https://github.com/rekrom/adsil-simulation/workflows/Code%20Coverage/badge.svg)](https://github.com/rekrom/adsil-simulation/actions)
[![C++](https://img.shields.io/badge/C++-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

ADSIL Analyzer is a comprehensive C++20 simulation platform for analyzing sensor-based geometries and signal interactions. It provides a modular architecture with enhanced logging, real-time visualization, and extensive JSON-based configuration support for automotive and sensor simulation scenarios.

## 🚀 Features

- 🧱 **Modular Architecture**: Independent CMake targets with clean interfaces
- 📊 **Enhanced Logging System**: High-performance logging with formatting, alignment, and environment configuration
- 🧮 **Geometric Modeling**: Advanced primitives with mesh and wireframe support
- 📡 **Sensor Simulation**: Comprehensive transmitter/receiver modeling with FOV calculations
- 🚗 **Vehicle Dynamics**: Car modeling and movement simulation
- 🧠 **Signal Processing**: Sophisticated signal solver framework for point cloud interactions
- 🖼️ **Real-time Visualization**: OpenGL-based 3D rendering with ImGui interface
- 📦 **JSON Integration**: Complete serialization/deserialization with adapters
- 🧪 **Test Coverage**: Comprehensive unit testing across all modules
- ⚡ **Performance Optimized**: 64% faster logging with zero-allocation string formatting

---

## 📁 Project Structure

```
adsil_analyzer_cpp/
├── apps/
│   └── adsil_analyzer/           # Main executable application
├── modules/                      # Modular libraries
│   ├── Adapter/                  # JSON serialization adapters
│   ├── Core/                     # Fundamental data structures & logging
│   ├── Geometry/                 # 3D shapes and geometric operations
│   ├── Math/                     # Mathematical utilities and algorithms
│   ├── Simulation/               # Physics simulation and scene management
│   ├── Spatial/                  # Spatial data structures and algorithms
│   ├── Utils/                    # Utility functions and helpers
│   ├── Vehicle/                  # Vehicle modeling and dynamics
│   └── Viewer/                   # OpenGL rendering and visualization
├── external/                     # Third-party dependencies
│   ├── glad/                     # OpenGL loader
│   ├── imgui/                    # Immediate mode GUI
│   ├── nlohmann_json/            # JSON library
│   └── tinyobjloader/            # OBJ file loader
├── resources/                    # Configuration files and assets
├── tools/                        # Development and analysis tools
├── docs/                         # Documentation
└── CMakeLists.txt               # Root build configuration
```

---

## 🛠️ Build Instructions

### Prerequisites

- **C++20** compatible compiler (GCC 8+, Clang 7+, MSVC 2019+)
- **CMake** ≥ 3.15
- **OpenGL** support (driver-dependent)
- **System Dependencies**:
  - Linux: `sudo apt install libglfw3-dev libglm-dev`
  - macOS: `brew install glfw glm`
  - Windows: Dependencies included via vcpkg or manual installation

### Quick Start

```bash
# Clone the repository
git clone https://github.com/rekrom/adsil-simulation.git
cd adsil-simulation

# Create build directory
mkdir build && cd build

# Configure with CMake (Release is default if not specified)
cmake ..

# Build (parallel compilation)
make -j$(nproc)  # Linux/macOS
# cmake --build . --parallel  # Cross-platform alternative

# Install into a local directory (recommended)
cmake --install . --prefix ../install_dir

# Run the application using the wrapper script
../install_dir/run_adsil.sh
```

### Environment Variables

The application requires the `ADSIL_RESOURCE_PATH` environment variable, which points to the `resources/` directory.
The provided `run_adsil.sh` script automatically sets this to the correct location.

- **Recommended (wrapper script)**

```bash
../install_dir/run_adsil.sh
```

- **Manual run (if you don’t want the wrapper script):**

```bash
export ADSIL_RESOURCE_PATH=/absolute/path/to/resources
../install_dir/bin/adsil_analyzer
```

### Build Options

```bash
# Debug build with full logging
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Release build with optimizations
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with tests
cmake .. -DBUILD_TESTING=ON

# Run tests
ctest --output-on-failure
```

### Binary Distribution (No Source Code)

To produce a tar.gz package containing only the stripped executable and resources (no source code):

```bash
mkdir -p build && cd build
cmake .. -DADSIL_DISTRIBUTION=ON -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
cpack  # generates adsil_analyzer-<version>-linux-x86_64.tar.gz
```

The archive layout:

```
bin/adsil_analyzer
resources/...
run_adsil.sh
```

Users run:

```bash
tar xf adsil_analyzer-<version>-linux-x86_64.tar.gz
cd adsil_analyzer
./run_adsil.sh
```

Environment variable `ADSIL_RESOURCE_DIR` is exported by the wrapper for runtime resource loading.

---

## 🧩 Module Overview

Each module is designed as a standalone library with minimal dependencies:

| Module         | Purpose                       | Key Features                           |
| -------------- | ----------------------------- | -------------------------------------- |
| **Core**       | Fundamental types and logging | Logger, Point, Vector, PointCloud      |
| **Math**       | Mathematical operations       | Linear algebra, transformations        |
| **Geometry**   | 3D shapes and modeling        | Primitives, meshes, shape factory      |
| **Spatial**    | Spatial data structures       | Octrees, spatial queries, optimization |
| **Vehicle**    | Vehicle dynamics and modeling | Car entities, movement simulation      |
| **Simulation** | Physics and scene management  | Scene loading, signal solving, physics |
| **Viewer**     | Real-time 3D visualization    | OpenGL rendering, ImGui interface      |
| **Adapter**    | JSON serialization            | Type adapters, configuration loading   |
| **Utils**      | Utility functions             | File I/O, string processing, helpers   |

---

## 🧪 Testing

The project includes comprehensive unit tests for all modules:

```bash
# Build with testing enabled
cmake .. -DBUILD_TESTING=ON
make

# Run all tests
ctest

# Run specific module tests
./bin/test_CoreTest
./bin/test_GeometryTest
./bin/test_AdapterTest
```

## 📊 Code Coverage

Test coverage is automatically tracked and reported via [Codecov](https://codecov.io/gh/rekrom/adsil-simulation). The coverage badge above shows the current coverage percentage.

### Coverage Reports

- **Adapter Module**: Comprehensive tests for JSON serialization/deserialization
- **Math Module**: Unit tests for geometric calculations and transformations
- **Geometry Module**: Tests for shape primitives and device modeling
- **Core Module**: Logger and utility function coverage
- **Simulation Module**: Scene management and signal processing tests

### Local Coverage Generation

```bash
# Install coverage tools
sudo apt-get install lcov gcovr

# Generate coverage report locally
./test_coverage_locally.sh

# View HTML coverage report
open coverage/html/index.html
```

The coverage workflow runs automatically on every push and pull request, generating detailed reports and updating the coverage badge.

---

## 📚 Documentation

- [Project Structure](docs/project_structure.md) - Detailed module breakdown
- [Build & Deployment Guide](docs/build_deployment_guide.md) - Advanced build options
- [UML Diagrams](docs/uml.drawio) - Architecture visualization

---

## 🔧 Development Tools

The project includes several development utilities:

```bash
# Code quality analysis
./tools/run-clang.tidy.sh

# Detect unused features
./tools/detect_unused_features.py

# Check for unused features
./tools/check_unused_features.sh
```

---

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

1. **Code Style**: Follow the existing code style and use clang-tidy
2. **Testing**: Add unit tests for new functionality
3. **Documentation**: Update relevant documentation
4. **Logging**: Use appropriate log levels and formatting

### Development Workflow

```bash
# Create feature branch
git checkout -b feature/new-feature

# Make changes and test
make && ctest

# Run code analysis
./tools/run-clang.tidy.sh

# Submit pull request
```

---

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- [nlohmann/json](https://github.com/nlohmann/json) - JSON library
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [tinyobjloader](https://github.com/tinyobjloader/tinyobjloader) - OBJ file loader

---

## 📈 Performance Metrics

- **Logging Performance**: 64% improvement over previous implementation
- **Memory Usage**: Zero allocations per log call
- **Build Time**: Parallel compilation support
- **Test Coverage**: Comprehensive unit testing across all modules
