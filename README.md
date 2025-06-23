# ADSIL Analyzer

ADSIL Analyzer is a modular C++17 simulation platform for analyzing sensor-based geometries and signal interactions. It provides a clean separation of core functionalities such as geometry modeling, device management, signal solving, and real-time OpenGL-based visualization.

## Features

- 🧱 Modular architecture with independent CMake targets
- 🧮 Geometric primitives with mesh and wireframe support (cube, cylinder, etc.)
- 📡 Sensor simulation (transmitters and receivers with FOV modeling)
- 🧠 Signal solver framework for point cloud interaction
- 🖼️ Real-time 3D rendering with OpenGL (via GLFW + GLAD)
- 📦 Easily extendable and reusable in other C++ projects

---

## Folder Structure

```
.
├── apps
│   └── adsil_analyzer        # Main executable app
├── modules
│   ├── Core                  # Vector, Point, PointCloud, etc.
│   ├── Device                # Transmitters, Receivers, FOV handling
│   ├── Geometry              # Shapes, Meshing, Shape Factory
│   ├── Simulation            # Scene management & signal solving
│   └── Viewer                # OpenGL renderers & camera
├── external
│   └── glad                  # OpenGL loader
├── include                   # Shared headers (optional)
├── CMakeLists.txt            # Root build script
└── README.md
```

---

## Build Instructions

### Requirements

- C++17 compiler
- CMake ≥ 3.15
- [GLFW](https://www.glfw.org/) (preinstalled system-wide or via package manager)
- [GLM](https://github.com/g-truc/glm) (header-only math library)
- OpenGL (driver support)

### Steps

```bash
# Clone and enter project
git clone https://github.com/yourusername/adsil_analyzer.git
cd adsil_analyzer

# Create a build directory
mkdir build && cd build

# Configure project
cmake ..

# Build project
make -j$(nproc)
```

### Run

```bash
./apps/adsil_analyzer/adsil_analyzer
```

---

## Module Overview

Each module is a standalone library and can be reused in other projects.

| Module     | Purpose                          |
| ---------- | -------------------------------- |
| Core       | Basic math types (Point, Vector) |
| Geometry   | Shape creation and meshing       |
| Device     | Transmitter/Receiver handling    |
| Simulation | Scene logic and solver pipeline  |
| Viewer     | Real-time 3D rendering           |

---

## License

MIT License. See `LICENSE` file for details.

---

## Contributing

This project is under active development. PRs and ideas for new modules (e.g., complex CAD shape importers, more accurate solvers) are welcome.
