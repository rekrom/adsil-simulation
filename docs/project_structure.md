adsil_analyzer_cpp/
├── apps/ # Application executables and main code
│ └── adsil_analyzer/ # Main analysis application
│ ├── main.cpp # Program entry point
│ ├── main.hpp # Main header file
│ └── CMakeLists.txt
├── external/ # Third-party libraries (vendor code)
│ ├── glad/ # GLAD OpenGL loader
│ └── nlohmann_json/ # JSON library (header-only)
├── modules/ # Modular project libraries
│ ├── Adapter/ # JSON adapters for serialization/deserialization
│ │ ├── include/ # Adapter header files
│ │ ├── src/ # Adapter source files
│ │ └── tests/ # Adapter unit tests
│ ├── Core/ # Core data structures and common infrastructure
│ │ ├── include/
│ │ └── src/
│ ├── Device/ # Hardware and sensor modeling
│ │ ├── include/
│ │ └── src/
│ ├── Geometry/ # Geometric shapes and physical modeling
│ │ ├── include/
│ │ ├── src/
│ │ └── tests/
│ ├── Simulation/ # Simulation logic and physics calculations
│ │ ├── include/
│ │ └── src/
│ ├── Utils/ # Utility functions, file operations, etc.
│ │ ├── include/
│ │ └── src/
│ └── Viewer/ # Visualization and rendering
│ ├── include/
│ └── src/
├── resources/ # Runtime data files (JSON, configs, etc.)
│ └── car.json # Example data file
├── CMakeLists.txt # Root-level CMake configuration
└── README.md # Project overview and usage guide

# **Folder Descriptions**

**apps/**
Contains the application executables. The adsil_analyzer folder holds the main program source and header files along with its CMake configuration.

**external/**
Third-party dependencies are kept here, separated from your own source code to ease updates and management.

**modules/**
The core functionality is split into modular libraries, each with its own include and source folders. Tests are colocated inside modules where applicable.

**resources/**
Contains external data files required at runtime like simulation configs, example JSONs, etc. These are not part of the build process.

**CMakeLists.txt**
The root CMake file orchestrates the build of all modules and applications.

**README.md**
The main project documentation, covering overview, build instructions, and basic usage.

**Notes & Best Practices**

- Each module follows a clear separation between include/ (headers) and src/ (implementation).

- Unit tests reside alongside the module source code under tests/.

- Use resources/ exclusively for files needed during runtime; do not mix with source.

- Maintain consistent naming conventions and folder hierarchy to ease onboarding and collaboration.

- New features or modules should follow the existing structure for scalability.
