<h1 align="center">
  C++ Graphics Engine & Software Rasterizer
</h1>

<p align="center">
  <i>A dependency-free, from-scratch 3D rendering engine and software rasterizer built in C++.</i>
</p>

<p align="center">
  <img src="docs/demo.gif" alt="Terminal rendering output demo" width="600"/>
  <br><i>[Demo 04_DynamicTerrain.cpp]</i>
</p>

---

## 🎯 Overview

This project provides an "open-box" implementation of a software rendering pipeline. While traditional graphics APIs (OpenGL/Vulkan/DirectX) hide the rasterization pipeline behind GPU hardware, this engine explicitly exposes the entire rendering loop on the CPU side. 

The architecture is built with a focus on:
- **Systems Design**: Clean abstractions, strictly modular layers, and entirely decoupled rendering payloads using C++ templates.
- **Mathematics & Algorithms**: Custom, from-scratch implementations of fundamental 3D transformations, Barycentric interpolation, geometric clipping, and matrix algebra.
- **Zero Dependencies**: Core mathematics and pipeline stages are crafted natively without relying on heavy external math or graphics libraries.

---

## 🚀 Core Architecture

### 1. Generic Payload Pipeline
The rendering engine mathematically processes generic data—whether it represents RGB colors, depth buffers, or custom vertex attributes. It achieves this by decoupling rendering logic from specific types (`ColorT`) via C++ templates. 
- Custom types can be rasterized simply by defining abstract behaviors in an `IInterpolator<ColorT>` interface.
- Core algorithms (lines, point projection, barycentric fills) remain payload-agnostic.

### 2. Custom Mathematical Foundation
To remain entirely self-contained, all linear algebra and coordinate systems are implemented from the ground up:
- Dedicated `Vector2`, `Vector3`, `Vector4`, and `Matrix3x3`, `Matrix4x4` structures.
- `Quaternion` mathematics for handling gimbal-lock-free 3D spatial rotations.
- Viewport and projection matrix construction (Orthographic, Perspective, Frustum).

### 3. Layered Composable Pipeline
The pipeline features strict boundaries, allowing developers to test, modify, or swap out individual stages without breaking the rendering loop:
- **Pixel Layer:** Abstract `IDrawingContext<ColorT>` interfaces bridged by protective wrappers (`ClippedGraphics`, `ViewportGraphics`).
- **Rasterization & Interpolation:** `RendererGraphics` constructs complex primitive meshes out of simple indexed vertex maps.
- **Dimensional Interfaces:** Separated geometric boundaries tailored explicitly to `IGraphics2D`, `IGraphics3D`, and `IGraphics4D` coordinate spaces.
- **High-Level Pipelines:** `Graphics2DPipeline` and `Graphics3DPipeline` coordinate matrix transform stacks (`PushMatrix`, `PopMatrix`), z-depth clipping, and full scene updates.

---

## 🛠️ Build & Installation

### Requirements
- **CMake** >= 3.16
- **C++11** compatible compiler (GCC / Clang / MSVC)
- *Zero external dependencies required.*

### Instructions
```bash
# 1. Clone the repository
git clone https://github.com/AdrianParry-17/GraphicsEngine.git
cd GraphicsEngine

# 2. Configure with CMake
cmake -S . -B build

# 3. Build the project
cmake --build build
```

---

## 🎮 Running The Examples

The examples output directly to the terminal using ASCII/variable-intensity block characters via a custom `TerminalBufferContext`. This keeps the test footprint extremely lightweight. (Resize your terminal window and decrease the font size for the best viewing experience!)

```bash
cd build

./01_Pipeline2D           # Basic 2D shapes and interpolations
./02_Pipeline3D           # Spinning 3D Cubes showcasing perspective projections
./03_CustomPipeline3D     # Exposes the manual, low-level pipeline component wiring
./04_DynamicTerrain       # Procedural rolling "wave/ocean" terrain
./05_SolarSystem          # Nested transformation matrices
```

---

## 🧪 Testing

The engine's architecture allows visual rendering steps to be decoupled from mathematical logic. A dedicated internal test harness ensures pipeline stability and regression safety.

```bash
# Build the test harness
cmake --build build --target GraphicsEngineTests

# Run the test suite
ctest --test-dir build --output-on-failure
```

Test coverage spans:
- Linear algebra, matrix projections, and Quaternion evaluations.
- Pipeline state checks and depth buffer sorting stability.
- Internal rasterization boundaries and geometric clipping rules.

---

## 📉 Known Limitations & Trade-offs

By prioritizing explicit architecture, modularity, and step-by-step readability over pure graphics throughput, the implementation makes a few deliberate trade-offs:
1. **Virtual Dispatch Overhead:** To maintain highly composable wrappers and an object-oriented design, rendering pipelines utilize virtual method resolution continuously at the pixel/fragment level.
2. **Perspective-Correct Interpolation:** Standard linear Barycentric interpolation thrives for arbitrary payloads like colors, but perspective mapping (UV textures) mathematically requires a scaler transformation step. A future extension of `IInterpolator<ColorT>` will be required to accommodate intermediate scaling calculations.
3. **Template Dimension Duplication:** The `Engine_2D`, `Engine_3D`, and `Engine_4D` structures isolate dimension-specific behaviors to keep coordinate math readable, which results in slightly duplicated pipeline abstractions.

---

## 📁 Repository Map

Key points of interest within the codebase:
- [`include/Engine/Engine_Graphics3DPipeline.h`](include/Engine/Engine_Graphics3DPipeline.h) - *The highest-level abstract representation of the 3D projection rendering loop.*
- [`include/Engine/Engine_Matrix.h`](include/Engine/Engine_Matrix.h) & [`src/Engine_Math.cpp`](src/Engine_Math.cpp) - *Core projection and mathematical transformation models.*
- [`include/Engine/Engine_Interpolation.h`](include/Engine/Engine_Interpolation.h) - *The template definitions decoupling geometry rasterization from rendering payloads.*