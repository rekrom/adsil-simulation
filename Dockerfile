# -------------------------------
# ADSIL Analyzer Dockerfile
# -------------------------------

# Base image (can be swapped to debian, fedora, archlinux, etc.)
FROM ubuntu:22.04

# Install build tools and dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git pkg-config \
    libglfw3-dev libglm-dev libx11-dev libxrandr-dev libxi-dev libxxf86vm-dev libxinerama-dev libxcursor-dev \
    mesa-utils mesa-common-dev libglu1-mesa-dev freeglut3-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy source code
COPY . .

# Configure & build (Release build by default)
RUN rm -rf build \
    && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --parallel

# Install into /install (like README instructions)
RUN cmake --install build --prefix /install

# Default workdir for running app
WORKDIR /install

# Wrapper script sets ADSIL_RESOURCE_PATH automatically
CMD ["./run_adsil.sh"]
