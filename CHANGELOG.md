# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2026-03-14

### Added
- **Texture System**: Added `Engine_Texture.h` with `ITexture<ColorT>` and composable adapters including `ContextTexture`, `FunctionTexture`, `TextureWrapper`, `ViewportTexture`, `ClippedTexture`, `RepeatedTexture`, and `ConvertTexture`.
- **Continuous Texture Sampling**: Added `Engine_Texture2D.h` with `ITexture2D<ColorT>`, `FunctionTexture2D`, `Texture2DWrapper`, `NativeTexture2D`, `ViewportTransformTexture2D`, and `NativeTextureAdaptMethod` for nearest-neighbor and bilinear sampling.
- **Textured Rendering Payloads**: Added `Engine_TextureGraphics.h` with `TexturedColor<ColorT>`, `TexturedColorInterpolator`, `TexturedColorScalableInterpolator`, and `TextureSamplingGraphics` for UV-aware rendering through the existing 2D and 3D pipelines.
- **New Examples**:
  - `06_TexturePipeline3D`: Demonstrates a textured rotating cube rendered through `Graphics3DPipeline` with perspective correction.
  - `07_TexturePipeline2D`: Demonstrates textured 2D geometry rendered through `Graphics2DPipeline`.

### Changed
- **README**: Updated feature overview, example list, and known limitations to document the new texturing workflow and current UV precision caveat.
- **Example Coverage**: The project examples now include both 2D and 3D texture sampling scenarios using a repeated checkerboard pattern.

### Known Limitations
- **Texture UV Precision**: Texture sampling near integer or upper-bound UV edges can still exhibit minor artifacts caused by floating-point precision and boundary mapping behavior. This is currently acknowledged and intentionally left unresolved.

## [1.1.0] - 2026-03-12

### Added
- **`Engine_WorldGraphics.h`**: New generic N-dimensional world graphics system (`WorldVertex<ColorT, D>`, `IWorldGraphics<ColorT, D>`, `MatrixWorldGraphics<ColorT, D>`, `PlaneClipWorldGraphics<ColorT, D>`, `ViewportTransformWorldGraphics<ColorT, D>`, etc.), replacing all prior hardcoded per-dimension duplicates.
- **`Engine_Transform.h`**: Extracted transformation-related static helper methods into dedicated `Transform2D` and `Transform3D` utility classes.
- **Generic Math Types**: `SquareMatrix<D>` and `Vector<D>` now support N-dimensional spaces; `Matrix2x2/3x3/4x4` and `Vector2/3/4` are retained as convenience typedefs.
- **`IScalableInterpolator`**: New interpolator interface extending `IInterpolator` with a `Scale(const T&, double, T&)` method, used when perspective correction is needed.
- **`ScalableInterpolator` / `FunctionScalableInterpolator`**: Concrete implementations of `IScalableInterpolator`.

### Changed
- **`Engine_2D/3D/4D.h`**: Now thin alias-only headers over the generic `Engine_WorldGraphics.h` types (e.g. `WorldVertex3D<ColorT>`, `IWorldGraphics3D<ColorT>`, `WorldPlane3D`).
- **`IInterpolator`**: Reverted to only requiring `Linear` and `Triangle`; scaling responsibility moved to `IScalableInterpolator`.
- **`Graphics3DPipeline`**: Now requires an `IScalableInterpolator` for color when perspective correction is enabled.
- **Vertex API**: Vertex positions are now stored as a `Vector<D> position` public field. Access uses `position.x()`, `position.y()`, `position.z()` instead of the old direct member fields `.x`, `.y`, `.z`.
- **Naming Convention**: World-space graphics classes renamed from `IGraphicsND`/`Vertex3D` style to `IWorldGraphics<ColorT, D>`/`WorldVertex<ColorT, D>` style to cleanly separate world-space from screen-space (`IGraphics`) concerns.
- Examples, tests, and all pipeline headers updated throughout to reflect the new API.

### Removed
- `IVectorInterpolator`, `VectorInterpolator`, `FunctionVectorInterpolator` from `Engine_Interpolation.h`.
- Hardcoded per-dimension vertex types `Vertex2D/3D/4D` and their `x`, `y`, `z`, `w` fields.
- Hardcoded per-dimension graphics interfaces `IGraphics2D/3D/4D`, `PlaneClippedGraphics2D/3D/4D`, `MatrixGraphics2D/3D/4D`, etc.  
- Transformation methods previously inlined on matrix types; now exclusively in `Engine_Transform.h`.

## [1.0.1] - 2026-03-03

### Added
- **Perspective Correction**: Implemented perspective-correct interpolation in 3D rendering pipeline.
- **Vector Interpolator**: Added `IVectorInterpolator` interface to handle simpler definition of interpolation for vector types using only scalar multiplication and addition.

### Changed
- **Terminal Output**: Updated `TerminalBufferContext` to map luminance values to ASCII brightness characters (` .:-=+*#%@`) rather than printing raw digits.
- **Interpolator Interface**: Extended the base `IInterpolator` interface with a `Scale(const T&, double, T&)` pure virtual function.
- **3D Example Generation**: The `02_Pipeline3D` example was restructured to properly render an isolated cube using 24 individual vertices and 36 indices.

### Fixed
- **Depth Buffer Projection Bug**: Resolved a clipping bug where `z_depth` buffer mapping always returned 0 due to an incorrect assignment order override.
## [1.0.0] - 2026-03-01

### Added
- **Core Math System**: Comprehensive math library including Vectors, Matrices, Quaternions, and Interpolation tailored for computer graphics.
- **Engine Core**: Base architectures and robust abstractions for 2D, 3D, and 4D spaces (`Engine_2D`, `Engine_3D`, `Engine_4D`).
- **Graphics Pipelines**: Implemented standardized `Engine_Graphics2DPipeline` and `Engine_Graphics3DPipeline` to process complex geometry, transformations, and rendering.
- **Render Context & Terminal Buffering**: Abstracted `Engine_Renderer` and `Engine_Context` alongside a robust `TerminalBufferContext` to display real-time graphical output directly and efficiently in the terminal.
- **Test Suite**: A massive and complete custom test harness, with fully polished tests checking mathematical correctness, matrix transforms, 2D/3D graphics pipelines, and world simulations.
- **Example Applications**:
  - `01_Pipeline2D`: Demonstrates standard 2D pipeline logic.
  - `02_Pipeline3D`: Demonstrates basic 3D projection, depth buffering, and rendering.
  - `03_CustomPipeline3D`: Showcases a custom 3D pipeline setup.
  - `04_DynamicTerrain`: Illustrates dynamic terrain generation algorithms and rendering.
  - `05_SolarSystem`: A full 3D simulation of a solar system using structural hierarchies.
- **Build System**: A polished, ready-to-test CMake configuration handling test compilation, core engine library targets, and granular executable generation for all examples.
