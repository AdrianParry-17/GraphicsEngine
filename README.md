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

This project provides an "open-box" implementation of a software rendering pipeline. While traditional graphics APIs (OpenGL/Vulkan/DirectX) hide most rasterization stages behind dedicated GPU hardware, this engine explicitly exposes the full rendering loop on the CPU side.

The goal is educational clarity **without sacrificing architectural rigor**: each stage is modular, testable, and replaceable, while still composing into a full 2D/3D renderer.

The architecture is built with a focus on:
- **Systems Design**: Clean abstractions, strictly layered components, and decoupled rendering payloads via templates.
- **Mathematics & Algorithms**: Custom implementations of transformations, clipping, barycentric interpolation, and matrix algebra.
- **Composable Texturing**: Reusable adapters and UV-aware payloads that integrate into the same core pipeline.
- **Zero Dependencies**: Core math and render stages are implemented natively (no heavyweight external graphics/maths libs).

---

## 🧭 Design Goals

1. **Explain the graphics pipeline explicitly**
   - Keep projection, clipping, rasterization, interpolation, and shading concerns visible and separable.
2. **Support payload-agnostic rendering**
   - Allow custom render payloads beyond simple RGB color.
3. **Prioritize composability over hardcoded specialization**
   - Use interfaces/wrappers so stages can be mixed, replaced, or tested in isolation.
4. **Retain practical performance characteristics for a CPU renderer**
   - Minimize accidental complexity while accepting intentional abstraction overhead.

---

## 🚀 Core Architecture

### 1. Generic Payload Pipeline
The renderer mathematically processes generic payload data (`ColorT`) rather than assuming a fixed pixel format.

- Custom payloads integrate through `IInterpolator<ColorT>`.
- Core primitives (lines, triangles, point projection) stay payload-agnostic.
- Perspective-correct workflows can opt into `IScalableInterpolator<ColorT>` for scaling-aware interpolation.

**Why this matters:** the same rasterization math works for plain colors, depth-aware payloads, or UV-carrying textured payload structs.

### 2. Custom Mathematical Foundation
To remain self-contained, linear algebra and coordinate utilities are implemented from scratch.

- Generic `Vector<D>` and `SquareMatrix<D>` with common aliases (`Vector2/3/4`, `Matrix2x2/3x3/4x4`).
- `Quaternion` support for robust 3D rotation composition.
- Transform helpers for translation/rotation/scaling plus orthographic/perspective/frustum projections.

**Why this matters:** behavior is deterministic, inspectable, and not constrained by third-party APIs.

### 3. Layered Composable Pipeline
The rendering stack is split into explicit layers with strict boundaries:

- **Pixel Layer**
  - `IDrawingContext<ColorT>` defines the draw surface contract.
  - Wrappers like `ClippedGraphics` and `ViewportGraphics` enforce safe draw regions.

- **Rasterization & Interpolation Layer**
  - `RendererGraphics` handles primitive assembly/rasterization.
  - Interpolator interfaces decouple geometry traversal from payload blending.

- **World Graphics Layer**
  - `IWorldGraphics<ColorT, D>` applies transforms, clipping, and viewport mapping in a dimension-generic way.
  - Convenience aliases are provided by `Engine_2D.h`, `Engine_3D.h`, and `Engine_4D.h`.

- **High-Level Pipeline Layer**
  - `Graphics2DPipeline` and `Graphics3DPipeline` orchestrate frame lifecycle and transform stack operations (`PushMatrix`, `PopMatrix`).

### 4. Texturing Pipeline
Texturing is additive and composable rather than hardwired into core rasterization.

- `ITexture<ColorT>` models discrete texel lookup.
- `ITexture2D<ColorT>` / `NativeTexture2D<ColorT>` adapt discrete textures into UV sampling space.
- `TexturedColor<ColorT>` carries base payload + UV attributes.
- `TextureSamplingGraphics` samples active textures during point emission and gracefully falls back to embedded payload color.

---

## 🔁 Frame Pipeline Walkthrough

A typical 3D frame flows through the following stages:

1. **Scene setup**
   - Update camera + object transforms.
   - Push world/model matrices to the pipeline stack.
2. **Vertex transform**
   - Convert model-space vertices into projected clip/screen-space representations.
3. **Geometric clipping**
   - Reject or trim primitives against configured clipping volumes.
4. **Triangle rasterization**
   - Iterate covered pixels using barycentric evaluation.
5. **Attribute interpolation**
   - Blend payload attributes (color, UV, custom data) per fragment.
6. **Optional texture sampling**
   - Resolve final payload from UVs when texturing wrappers are active.
7. **Depth/order handling + draw output**
   - Submit to drawing context (terminal buffer in examples).

This explicit flow is intentionally readable and debuggable, making it suitable for learning or for building custom software rendering experiments.

---

## 🛠️ Build & Installation

### Requirements
- **CMake** >= 3.16
- **C++11** compatible compiler (GCC / Clang / MSVC)
- No external runtime dependencies

### Build
```bash
# 1. Clone the repository
git clone https://github.com/AdrianParry-17/GraphicsEngine.git
cd GraphicsEngine

# 2. Configure with CMake
cmake -S . -B build

# 3. Build all targets
cmake --build build
```

### Optional: build a specific config (multi-config generators)
```bash
cmake --build build --config Release
```

---

## 🎮 Running The Examples

Examples render to the terminal through a custom `TerminalBufferContext` using ASCII / block-intensity characters.

> Tip: maximize terminal size and reduce font scale for smoother perceived resolution.

```bash
cd build

./01_Pipeline2D           # Basic 2D primitives and interpolation
./02_Pipeline3D           # Rotating 3D cubes with perspective projection
./03_CustomPipeline3D     # Manual low-level pipeline composition
./04_DynamicTerrain       # Procedural rolling terrain/wave surface
./05_SolarSystem          # Hierarchical transforms (parent-child matrix stack)
./06_TexturePipeline3D    # UV texturing on a rotating cube
./07_TexturePipeline2D    # Textured quad through the 2D path
```

---

## 🧪 Testing

The repository includes an internal test harness that validates mathematical behavior and core pipeline invariants.

```bash
# Build tests
cmake --build build --target GraphicsEngineTests

# Run tests
ctest --test-dir build --output-on-failure
```

Coverage includes:
- Vector/matrix/quaternion correctness
- Projection + transform consistency
- Rasterization and clipping boundary behavior
- Pipeline state progression and depth ordering stability

---

## 🧩 Extending the Engine

### Add a custom payload type
1. Define a payload struct (e.g., color + extra attributes).
2. Implement `IInterpolator<YourPayload>`.
3. If perspective-correct scaling is required, also implement `IScalableInterpolator<YourPayload>`.
4. Route rendering through existing pipeline/wrapper layers.

### Add a custom drawing backend
1. Implement `IDrawingContext<ColorT>`.
2. Optionally wrap with clipping/viewport decorators.
3. Reuse the same world/rasterization pipeline unchanged.

### Add a custom texture source
1. Implement or adapt to `ITexture<ColorT>`.
2. Wrap with `ITexture2D<ColorT>` or `NativeTexture2D<ColorT>` for UV sampling.
3. Use `TextureSamplingGraphics` to bind texturing into draw output.

---

## 📉 Known Limitations & Trade-offs

By prioritizing explicit architecture and readability over raw throughput, the engine intentionally accepts several trade-offs:

1. **Virtual Dispatch Overhead**
   - Composability relies on interfaces/wrappers, which introduces per-fragment dynamic dispatch costs.
2. **Perspective-Correct Interpolation Contract**
   - Perspective correction requires scalable interpolation semantics via `IScalableInterpolator<ColorT>`.
3. **Generic API Verbosity**
   - Generic `Vector<D>` design removes duplication but can be more verbose than dimension-specialized field layouts.
4. **Texture UV Precision Edges**
   - Minor sampling artifacts may appear at UV boundaries due to floating-point edge conditions.

---

## 📁 Repository Map

Key entry points for understanding the codebase:

- [`include/Engine/Engine_Graphics3DPipeline.h`](include/Engine/Engine_Graphics3DPipeline.h)
  High-level 3D rendering loop orchestration.
- [`include/Engine/Engine_WorldGraphics.h`](include/Engine/Engine_WorldGraphics.h)
  Dimension-agnostic world graphics abstraction (`IWorldGraphics<ColorT, D>`).
- [`include/Engine/Engine_Matrix.h`](include/Engine/Engine_Matrix.h) and [`src/Engine_Math.cpp`](src/Engine_Math.cpp)
  Core matrix/math implementation.
- [`include/Engine/Engine_Transform.h`](include/Engine/Engine_Transform.h)
  Transform and projection helpers.
- [`include/Engine/Engine_Interpolation.h`](include/Engine/Engine_Interpolation.h)
  Interpolation contracts for payload-agnostic rasterization.
- [`include/Engine/Engine_TextureGraphics.h`](include/Engine/Engine_TextureGraphics.h)
  Texture payload types, interpolators, and sampling wrappers.

---

## 🤝 Contributing

Contributions are welcome, especially around:
- Additional sample scenes
- Profiling/performance instrumentation
- New drawing contexts (image output, GUI windows, etc.)
- Expanded test coverage and edge-case validation

If you are experimenting with new pipeline stages, prefer adding them as wrappers/adapters so they remain composable with existing architecture.
