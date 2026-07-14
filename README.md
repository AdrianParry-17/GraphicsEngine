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
- **Reusable Mesh Data**: Generic indexed meshes and built-in 2D/3D primitive generators integrate with the same payload-independent rendering path.
- **Composable Texturing**: Reusable texture adapters, UV-aware payloads, and texture sampling layers that plug into the same generic 2D/3D rendering pipelines.
- **Zero Dependencies**: Core mathematics and pipeline stages are crafted natively without relying on heavy external math or graphics libraries.

---

## 🚀 Core Architecture

### 1. Generic Payload Pipeline
The rendering engine mathematically processes generic data—whether it represents RGB colors, depth buffers, or custom vertex attributes. It achieves this by decoupling rendering logic from specific types (`ColorT`) via C++ templates. 
- Custom types can be rasterized simply by defining abstract behaviors in an `IInterpolator<ColorT>` interface.
- Core algorithms (lines, point projection, barycentric fills) remain payload-agnostic.

### 2. Custom Mathematical Foundation
To remain entirely self-contained, all linear algebra and coordinate systems are implemented from the ground up:
- Generic N-dimensional `Vector<D>` and `SquareMatrix<D>` types; `Vector2/3/4` and `Matrix2x2/3x3/4x4` are convenience aliases.
- `Quaternion` mathematics for handling gimbal-lock-free 3D spatial rotations.
- Viewport and projection matrix construction (Orthographic, Perspective, Frustum) via `Transform2D` / `Transform3D` utility classes.

### 3. Layered Composable Pipeline
The pipeline features strict boundaries, allowing developers to test, modify, or swap out individual stages without breaking the rendering loop:
- **Pixel Layer:** Abstract `IDrawingContext<ColorT>` interfaces bridged by protective wrappers (`ClippedGraphics`, `ViewportGraphics`).
- **Rasterization & Interpolation:** `RendererGraphics` rasterizes points, lines, and triangles while interpolating generic vertex payloads.
- **World Graphics Layer:** A unified, N-dimensional `IWorldGraphics<ColorT, D>` abstraction covers vertex transformation, clipping, and viewport mapping generically. `Engine_2D/3D/4D.h` provide convenient dimension-specific aliases (`IWorldGraphics3D`, `WorldVertex3D`, etc.).
- **High-Level Pipelines:** `Graphics2DPipeline` and `Graphics3DPipeline` coordinate matrix transform stacks (`PushMatrix`, `PopMatrix`), z-depth clipping, and full scene updates.

### 4. Mesh Data
`Mesh<ColorT, D>` is a reusable vertex-and-index container that works with the existing generic payload pipeline. `MeshPrimitive` provides common 2D and 3D shapes, and each generator supports both a convenient return-by-value form and an output-parameter form for reusing a mesh.

```cpp
Engine::Mesh3D<double> cube = Engine::MeshPrimitive::GenerateCube3D(1.0);
pipeline.RenderMesh(cube);
```

### 5. Texturing Pipeline
Texturing is implemented as another composable layer rather than as a hardcoded special case:
- **Discrete Textures:** `ITexture<ColorT>` models integer-coordinate texture lookups and can be backed by contexts, functions, clipped regions, repeated regions, or converted payload types.
- **Continuous UV Sampling:** `ITexture2D<ColorT>` and `NativeTexture2D<ColorT>` adapt integer textures into floating-point UV space using nearest-neighbor or bilinear filtering.
- **Texture Edge Handling:** `NativeTexture2D<ColorT>` can use an explicit source rectangle with `EdgeHandlingMode::Clamp`, `Repeat`, or `Reject` so UV edge samples have defined behavior.
- **Textured Payloads:** `TexturedColor<ColorT>` carries both the base color and UV coordinates, while `TexturedColorInterpolator` / `TexturedColorScalableInterpolator` preserve those attributes during interpolation.
- **Rendering Hook:** `TextureSamplingGraphics` samples the active texture during `DrawPoint()` and falls back to the embedded color when texturing is disabled or sampling fails.

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
./06_TexturePipeline3D    # Textured rotating cube with checkerboard UV sampling
./07_TexturePipeline2D    # Textured 2D quad rendered through the 2D pipeline
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
- Texture sampling, source-area edge policies, and cross-dimension vector comparisons.

Mesh primitives also have standalone single-frame terminal render checks:

```bash
cmake --build build --target MeshRenderTest AdditionalMeshesRenderTest

./build/MeshRenderTest
./build/AdditionalMeshesRenderTest
```

`MeshRenderTest` renders a cube. `AdditionalMeshesRenderTest` renders the tetrahedron, octahedron, cylinder, cone, and sphere together for visual inspection.

---

## 📉 Known Limitations & Trade-offs

By prioritizing explicit architecture, modularity, and step-by-step readability over pure graphics throughput, the implementation makes a few deliberate trade-offs:
1. **Virtual Dispatch Overhead:** To maintain highly composable wrappers and an object-oriented design, rendering pipelines utilize virtual method resolution continuously at the pixel/fragment level.
2. **Perspective-Correct Interpolation:** Perspective-correct rasterization requires a scalar scaling step on the color payload. This is now cleanly separated from general interpolation via a dedicated `IScalableInterpolator<ColorT>` interface. Callers who need perspective correction must provide this extended type; those who do not can continue using the simpler `IInterpolator<ColorT>`.
3. **Generic API Verbosity:** The move from hardcoded per-dimension types to a generic `Vector<D>`-based system resolves dimension duplication entirely, but introduces slightly more verbose position access. Where the old API allowed `vertex.x`, `vertex.y`, `vertex.z`, the new API requires `vertex.position.x()`, `vertex.position.y()`, `vertex.position.z()`. Dimension-specific constructors and accessor helpers are provided to mitigate this.
4. **Explicit Texture Source Bounds:** The generic `ITexture<ColorT>` contract intentionally has no width/height requirement. `NativeTexture2D<ColorT>` applies `Clamp` / `Repeat` / `Reject` edge handling only when configured with a source rectangle; unbounded texture callbacks keep their direct sampling behavior.

---

## 📁 Repository Map

Key points of interest within the codebase:
- [`include/Engine/Engine_Graphics3DPipeline.h`](include/Engine/Engine_Graphics3DPipeline.h) - *The highest-level abstract representation of the 3D projection rendering loop.*
- [`include/Engine/Engine_WorldGraphics.h`](include/Engine/Engine_WorldGraphics.h) - *The generic N-dimensional world graphics system; the core of the dimension-agnostic pipeline architecture.*
- [`include/Engine/Engine_Mesh.h`](include/Engine/Engine_Mesh.h) - *The generic indexed triangle mesh data type and `RenderMesh` integration.*
- [`include/Engine/Engine_MeshPrimitive.h`](include/Engine/Engine_MeshPrimitive.h) - *Built-in 2D and 3D primitive mesh generators.*
- [`include/Engine/Engine_Matrix.h`](include/Engine/Engine_Matrix.h) & [`src/Engine_Math.cpp`](src/Engine_Math.cpp) - *Core N-dimensional matrix math and projection model.*
- [`include/Engine/Engine_Transform.h`](include/Engine/Engine_Transform.h) - *Static transformation helpers (translation, rotation, scale, projection) for 2D and 3D spaces.*
- [`include/Engine/Engine_Interpolation.h`](include/Engine/Engine_Interpolation.h) - *The `IInterpolator` and `IScalableInterpolator` interfaces decoupling rasterization from rendering payloads.*
- [`include/Engine/Engine_TextureGraphics.h`](include/Engine/Engine_TextureGraphics.h) - *Textured payload/interpolator utilities and the graphics wrapper that performs runtime texture sampling.*
