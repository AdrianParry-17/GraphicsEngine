# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
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
