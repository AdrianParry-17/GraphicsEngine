# GraphicsEngine Codebase Documentation

This document is a complete technical walkthrough of the current codebase for people who are new to the project.

---

## 1) What this project is

`GraphicsEngine` is a header-only C++ rendering toolkit (plus a demo `Main.cpp`) that implements:

- Pixel plotting abstractions
- 2D primitive drawing
- Color interpolation across lines and triangles
- 2D / 3D / 4D vertex pipelines
- Matrix-based transforms (2x2, 3x3, 4x4)
- Geometric clipping against planes (2D/3D/4D)
- A higher-level 2D and 3D pipeline API
- A text-mode ASCII demo renderer in `Main.cpp`

All engine logic lives under `include/Engine/` as templates and inline code.

---

## 2) Build and run

Project file: `CMakeLists.txt`

- C++ standard: `C++11`
- Executable target: `GraphicsEngine`
- Source entry point: `src/Main.cpp`
- Include path: `include`

Typical flow:

1. Configure with CMake.
2. Build target `GraphicsEngine`.
3. Run executable.

The demo currently chooses 2D or 3D test based on `is_3d` in `main()`.

---

## 3) High-level architecture

The architecture is layered from low-level pixel output up to higher-level pipelines:

1. **Drawing context layer** (`IDrawingContext`)  
	Raw pixel read/write interface.

2. **Graphics layer** (`IGraphics`, wrappers, clipping, conversion)  
	Pixel plotting and primitive rasterization.

3. **Renderer layer** (`RendererGraphics`)  
	Adds interpolation-aware line and triangle filling.

4. **Dimensional graphics interfaces** (`IGraphics2D/3D/4D`)  
	Render triangles using vertex structs.

5. **Adapters and transforms** (projection/embed/convert/matrix/clipping in 2D/3D/4D headers)  
	Move geometry between spaces and apply transformations.

6. **Pipeline classes** (`Graphics2DPipeline`, `Graphics3DPipeline`)  
	Compose all above parts into easy-to-use rendering pipelines.

---

## 4) File-by-file guide

### `include/Engine/Engine.h`

Aggregator header that includes all engine modules.

Use this single include when you want full engine functionality.

---

### `include/Engine/Engine_Constant.h`

Defines numeric tolerance helpers in `Engine::NumericConstants`:

- `NearZero = 1E-8`
- `IsNearZero(x)`
- `IsNearZeroOrNegative(x)` (checks `x < NearZero`)
- `IsNearZeroOrPositive(x)` (checks `x > -NearZero`)

These are heavily used for stability checks in transforms and clipping.

---

### `include/Engine/Engine_Context.h`

Defines the core output interface:

```cpp
template <typename ColorT>
struct IDrawingContext {
	 virtual bool SetPixel(int x, int y, const ColorT& color) = 0;
	 virtual bool GetPixel(int x, int y, ColorT& res) = 0;
};
```

This is the lowest-level abstraction the rest of the renderer can target.

---

### `include/Engine/Engine_Geometry.h`

Defines integer geometry utilities:

- `Point { int x, y; }`
- `Size { int w, h; }`
- `Rectangle { int x, y, w, h; }`

Main characteristics:

- Rich arithmetic operators (`+`, `-`, `*`, compound ops, comparisons)
- Rectangle supports positive or negative width/height
- Rectangle methods normalize bounds through `LeftSide()/RightSide()/TopSide()/BottomSide()`
- Includes containment, intersection, union, coordinate conversion helpers

Important rectangle semantics:

- Coordinates are **inclusive** on both ends when using side methods.
- Width/height of `1` means one pixel span.
- `IsEmptyArea()` is true when `w == 0 || h == 0`.

---

### `include/Engine/Engine_Vector.h`

Defines:

- `Vector2`
- `Vector3`
- `Vector4`

Features:

- Arithmetic operators and scalar operations
- Dot/cross products
- Magnitude and normalization
- Conversions between vector types and geometry types (`Point`, `Size`)

Notes:

- `Normalize()` methods return zero vector if magnitude is near zero.
- `Vector2 * Vector2` returns a `Vector3` (z-only cross product form).
- Operations are math-oriented and used widely by matrix and pipeline code.

---

### `include/Engine/Engine_Quaternion.h`

Quaternion type for 3D rotations:

- Components: `a` (real), `b,c,d` (imaginary)
- Quaternion multiplication and scalar ops
- `Conjugate()`, `Norm()`, `Unit()`, `Reciprocal()`
- `CreateRotation(rx, ry, rz)` builds quaternion from Euler angles

Used primarily by matrix helpers to produce rotation matrices.

---

### `include/Engine/Engine_Interpolation.h`

Interpolation abstraction:

- `IInterpolator<T>` with:
  - `Linear(a, b, t, result)`
  - `Triangle(a, b, c, wa, wb, wc, result)`

Implementations:

- `Interpolator<T>`: arithmetic interpolation assuming type supports weighted operations
- `FunctionInterpolator<T>`: inject custom interpolation lambdas

This is how the engine interpolates colors and fragment attributes.

---

### `include/Engine/Engine_Graphics.h`

Defines the pixel and primitive drawing framework.

#### Core interfaces and wrappers

- `IGraphics<ColorT>`: single method `DrawPoint(x, y, color)`
- `GraphicsWrapper<ColorT>`: forwards to another `IGraphics`
- `ContextGraphics<ColorT>`: bridges `IDrawingContext` to `IGraphics`
- `FunctionGraphics<ColorT>`: uses callback as a graphics target

#### Spatial wrappers

- `ViewportGraphics<ColorT>`
  - Interprets input coordinates as local viewport coordinates
  - Offsets output to global space
- `ClippedGraphics<ColorT>`
  - Forwards only points inside a clipping `Rectangle`

#### Color conversion wrapper

- `ConvertedGraphics<ColorTIn, ColorTOut>`
  - Converts color type before forwarding to another graphics target

#### Primitive rasterizer

`PrimitiveGraphics<ColorT>` provides:

- Point
- Horizontal/vertical lines
- General lines (step-based sampling with rounding)
- Triangle outlines
- Filled rectangles
- Filled triangles (scanline-like by edge sampling)

`FillTriangle` behavior summary:

1. Rasterize triangle edges into per-row x-ranges.
2. Fill each row using horizontal lines.

---

### `include/Engine/Engine_Renderer.h`

`RendererGraphics<ColorT>` extends `GraphicsWrapper` and adds interpolation-aware rendering.

Key methods:

- `RenderPoint`
- `RenderLine`, `RenderLineExcludeStart`, `RenderLineExcludeEnd`
- `RenderTriangle`

Triangle rendering handles:

- Degenerate point case
- Degenerate line cases (duplicate vertices / collinear)
- General filled triangle with barycentric interpolation

General triangle path:

1. Compute triangle area term (`cross_prod`).
2. For each filled pixel, compute barycentric weights.
3. Interpolate color with `interpolator.Triangle`.
4. Draw interpolated pixel.

---

### `include/Engine/Engine_2D.h`

2D vertex and graphics pipeline building blocks.

#### Types and interfaces

- `Vertex2D<ColorT>` (`x`, `y`, `color`)
- `IGraphics2D<ColorT>` with:
  - `RenderTriangle(a,b,c)` (pure virtual)
  - `RenderGeometry(vertices, triangleIndices)` (default index traversal)

#### Wrappers/adapters

- `RendererGraphics2D<ColorT>`: adapts 2D triangles to `RendererGraphics`
- `Graphics2DWrapper<ColorT>`: forwarding wrapper
- `ConvertedGraphics2D<ColorTIn, ColorTOut>`: per-vertex conversion
- `PositionConvertedGraphics2D<ColorT>` alias

#### Transform and matrix classes

- `ViewportTransformGraphics2D<ColorT>`
  - Maps coordinates from input viewport to output viewport
  - Applies transform per vertex before forwarding
- `MatrixGraphics2D<ColorT>`
  - Maintains a `std::stack<Matrix2x2>`
  - Supports push/pop/load identity
  - Supports multiply/scale/rotate
  - Applies transform to vertices before rendering

#### Clipping

- `Plane2D`: line/plane represented by `a*x + b*y + c = 0`
- `PlaneClippedGraphics2D<ColorT>`
  - Clips triangles against a list of `Plane2D`
  - Uses Sutherland–Hodgman style polygon clipping
  - Interpolates color at intersections
  - Triangulates resulting polygon as fan `(0, i-1, i)`

---

### `include/Engine/Engine_3D.h`

3D counterparts to the 2D system.

#### Types/interfaces

- `Vertex3D<ColorT>`
- `IGraphics3D<ColorT>` (`RenderTriangle`, default indexed `RenderGeometry`)

#### Adapters

- `Projected2DGraphics3D<ColorTIn, ColorTOut>`: 3D -> 2D projection
- `Embed3DGraphics2D<ColorTIn, ColorTOut>`: 2D -> 3D embedding
- `ConvertedGraphics3D<ColorTIn, ColorTOut>`: per-vertex conversion
- `Graphics3DWrapper<ColorT>` forwarding wrapper

#### Matrix transform

- `MatrixGraphics3D<ColorT>` uses `Matrix3x3` stack
- Supports:
  - 2D homogeneous transforms (`Translate2D`, `Scale2D`, `Rotate2D`, `Transform2D`, `ViewportTransform2D`)
  - 3D linear transforms without translation (`Scale3D`, `Rotate3D`)

#### Clipping

- `Plane3D` with equation `a*x + b*y + c*z + d = 0`
- `PlaneClippedGraphics3D<ColorT>` triangle clipping with color interpolation

---

### `include/Engine/Engine_4D.h`

4D stage used for homogeneous transformation and clipping before projection to 3D.

#### Types/interfaces

- `Vertex4D<ColorT>`
- `IGraphics4D<ColorT>`

#### Adapters

- `Projected3DGraphics4D<ColorTIn, ColorTOut>`: 4D -> 3D
- `Embed4DGraphics3D<ColorTIn, ColorTOut>`: 3D -> 4D
- `ConvertedGraphics4D<ColorTIn, ColorTOut>`
- `Graphics4DWrapper<ColorT>`

#### Matrix transform

- `MatrixGraphics4D<ColorT>` with `Matrix4x4` stack
- Supports:
  - `Translate3D`, `Scale3D`, `Rotate3D`, `Transform3D`
  - Projection transforms: `Frustum`, `Perspective`, `Orthographic`

#### Clipping

- `Plane4D` with equation `a*x + b*y + c*z + d*w + e = 0`
- `PlaneClippedGraphics4D<ColorT>` for clip-space clipping

---

### `include/Engine/Engine_Matrix.h`

Defines `Matrix2x2`, `Matrix3x3`, `Matrix4x4`.

Shared characteristics:

- Row-major storage in `std::array`
- Matrix-matrix arithmetic and multiplication
- Matrix-vector multiplication
- Utility constructors/generators for identity and transforms

#### `Matrix2x2`

- 2D linear transforms (scale/rotation)
- Used by `MatrixGraphics2D`

#### `Matrix3x3`

- 2D homogeneous transforms (translate/rotate/scale)
- Viewport transform matrix generator
- 3D linear transforms via quaternion-based rotation and scale
- Used by `MatrixGraphics3D`

#### `Matrix4x4`

- 3D homogeneous transforms
- Projection matrices:
  - Frustum
  - Perspective
  - Orthographic

Conventions inferred from implementation:

- Vectors treated as column vectors in multiplication `M * v`
- Composition for local transform calls is post-multiply in stack class (`top *= mat`)
- Built helper `GetTransformation3D` and `GetTransformation2D` encode Scale -> Rotate -> Translate intent

Stability behavior:

- Invalid numeric parameters generally return identity matrices rather than throwing.

---

### `include/Engine/Engine_Graphics2DPipeline.h`

High-level 2D rendering pipeline class: `Graphics2DPipeline<ColorT>`.

#### Internal fragment

`Fragment` contains only `color`.

#### Internal chain (from input to output)

1. `ConvertedGraphics2D<ColorT, Fragment>` (`input_g2d`)
2. `Embed3DGraphics2D<Fragment, Fragment>` (2D -> 3D with `z=1`)
3. `MatrixGraphics3D<Fragment>` (transform stage)
4. `Projected2DGraphics3D<Fragment, Fragment>` (homogeneous divide by `z`)
5. `PlaneClippedGraphics2D<Fragment>` (world clipping)
6. `ViewportTransformGraphics2D<Fragment>` (world -> viewport)
7. `RendererGraphics2D<Fragment>` -> `RendererGraphics<Fragment>`
8. `FunctionGraphics<Fragment>` callback into `drawFragment`
9. User-provided `IGraphics<ColorT>` target

#### Public capabilities

- Viewport management (`SetViewport`, `GetViewport`)
- Camera/world setup (`SetWorldCamera(height, aspect)`)
- Matrix stack operations delegated to internal `matrix_g3d`
- Render triangle/geometry

#### Defaults

- Default clip planes represent world boundary around `[-1, 1]` in x and y.

---

### `include/Engine/Engine_Graphics3DPipeline.h`

High-level 3D rendering pipeline class: `Graphics3DPipeline<ColorT>`.

#### Internal fragment

`Fragment` contains:

- `color`
- `z_depth` for optional depth testing

#### Internal chain

1. `ConvertedGraphics3D<ColorT, Fragment>` (`input_g3d`)
2. `Embed4DGraphics3D<Fragment, Fragment>` (3D -> 4D, set `w=1`)
3. `MatrixGraphics4D<Fragment>` (model/view/projection transforms)
4. `PlaneClippedGraphics4D<Fragment>` (clip in homogeneous clip space)
5. `Projected3DGraphics4D<Fragment, Fragment>` (divide by `w`)
6. `Projected2DGraphics3D<Fragment, Fragment>` (drop/forward z for depth)
7. `ViewportTransformGraphics2D<Fragment>`
8. `RendererGraphics2D<Fragment>` -> `RendererGraphics<Fragment>`
9. `FunctionGraphics<Fragment>` callback into `drawFragment`
10. User `IGraphics<ColorT>`

#### Depth buffer

- Stored as `std::vector<double> depth_buffer`
- Enabled by `EnableDepthBuffer()`
- Cleared to `INFINITY`
- Test: if incoming `z_depth > currentDepth`, fragment is discarded

#### Clipping defaults

Default 4D clip planes correspond to canonical clip-space constraints:

- `-x + w >= 0`, `x + w >= 0`
- `-y + w >= 0`, `y + w >= 0`
- `-z + w >= 0`, `z + w >= 0`

#### Public capabilities

- Viewport, matrix stack, projection setup, transforms, depth buffer controls
- Render triangle/geometry

---

## 5) Demo program (`src/Main.cpp`)

`Main.cpp` is an ASCII visualization harness.

### `BufferContext`

Implements `IDrawingContext<double>` using 2D `std::vector<double>`.

Responsibilities:

- Store pixel intensity per cell
- Bounds-check writes/reads
- Count out-of-bounds requests (`out_bound`)
- `Write()` prints buffer as bordered ASCII frame
  - `0` displayed as space
  - `1..9` displayed as digit chars

### Geometry generation

- `RenderCube(IGraphics3D<double>&)` renders cube via 12 triangles
- `RenderCube_pipe(Graphics3DPipeline<double>&)` same but calls pipeline directly

### Demo functions

#### `test_pipeline3d()`

- Uses `Graphics3DPipeline<double>`
- Sets perspective projection
- Enables depth buffer
- Animates two cubes with transforms across frames

#### `simulate3d()`

- Manual composition of low-level components (without pipeline wrapper)
- Useful as an educational example of how the chain is assembled

#### `test_pipeline2d()`

- Uses `Graphics2DPipeline<double>`
- Sets world camera
- Renders two rotating squares with interpolated vertex values

### `main()`

- Toggles between 2D and 3D test using `const bool is_3d`

---

## 6) How data flows in practice

For a single triangle submitted to `Graphics3DPipeline::RenderTriangle`:

1. Input vertex color is wrapped into `Fragment` (plus initial z depth).
2. Vertex is embedded into 4D (`w=1`).
3. Matrix stack transforms applied (model/view/projection style).
4. 4D triangle clipped against clip-space planes.
5. Vertex projected back to 3D by dividing by `w`.
6. 3D projected to 2D viewport coordinates.
7. Rasterization happens in `RendererGraphics` with interpolated fragment attributes.
8. `drawFragment` performs optional depth test, then writes final pixel color.

2D pipeline is analogous but uses a 2D-focused chain.

---

## 7) Public API quick-start

### Minimal 2D pipeline setup

```cpp
Engine::ContextGraphics<double> ctxGraphics(context);
Engine::Interpolator<double> interp;
Engine::Graphics2DPipeline<double> p2d(ctxGraphics, interp, Engine::Rectangle(0, 0, width, height));

p2d.SetWorldCamera(worldHeight, aspect);
p2d.LoadIdentity();
p2d.Transform2D({tx, ty}, rotation, {sx, sy});
p2d.RenderTriangle(v0, v1, v2);
```

### Minimal 3D pipeline setup

```cpp
Engine::ContextGraphics<double> ctxGraphics(context);
Engine::Interpolator<double> interp;
Engine::Graphics3DPipeline<double> p3d(ctxGraphics, interp, Engine::Rectangle(0, 0, width, height));

p3d.LoadIdentity();
p3d.Perspective(fovY, aspect, near, far);
p3d.EnableDepthBuffer();

p3d.PushMatrix();
p3d.Transform3D({tx, ty, tz}, {rx, ry, rz}, {sx, sy, sz});
p3d.RenderTriangle(v0, v1, v2);
p3d.PopMatrix();
```

---

## 8) Known behaviors and caveats (important for contributors)

1. **Header-only style**  
	Most logic is inline templates; compile times can grow as usage grows.

2. **Matrix stack safeguards**  
	`PopMatrix` and `LoadIdentity` are defensive: they recover identity if stack is empty.

3. **Projection parameter validation**  
	Matrix projection helpers return identity on invalid input rather than throwing.

4. **Pipeline viewport validation currently checks existing member**  
	In `SetViewport` of both pipeline classes, the emptiness check currently references `viewport` (existing member) rather than `_viewport` argument.

5. **`RendererGraphics` line interpolation index**  
	The internal `pixel_i` variable is captured in callbacks but not incremented inside draw callbacks; this means line gradient behavior may not match intended per-pixel progression.

6. **3D depth policy**  
	Depth test uses `frag.z_depth > depth` as "farther" rejection. Interpretation depends on projection convention and z mapping.

7. **No shading/light model**  
	Color interpolation is attribute-only; no built-in normals/lighting pipeline.

8. **No culling stage**  
	Backface culling is not implemented in pipeline wrappers.

---

## 9) Extending the engine safely

Recommended extension points:

- Add custom color/fragment interpolation via `FunctionInterpolator`
- Add new wrappers around `IGraphics`, `IGraphics2D/3D/4D`
- Add culling stage before rasterization in pipelines
- Add texture attributes in `Fragment` and interpolate them
- Add perspective-correct interpolation in 3D fragment stage

When extending:

- Keep vertex-space transitions explicit (2D <-> 3D <-> 4D).
- Keep clip and projection ordering intact.
- Preserve matrix/vector convention consistency (`M * v` row-major storage implementation).

---

## 10) Glossary (project-specific)

- **Graphics**: pixel-level plotting interface (`IGraphics`)
- **RendererGraphics**: interpolation-capable triangle rasterizer
- **IGraphics2D/3D/4D**: triangle interfaces for different dimensional spaces
- **Embed**: increase dimensionality (e.g., 3D -> 4D by setting `w=1`)
- **Project**: reduce dimensionality (e.g., divide by homogeneous coordinate)
- **Plane clipper**: polygon clip stage against half-spaces
- **Fragment**: per-pixel data payload (color + optional attributes)
- **Viewport transform**: map world/NDC coordinates to output pixel rectangle

---

## 11) Suggested onboarding order for new developers

1. Read `Engine_Graphics.h` and `Engine_Renderer.h` first.
2. Study `Engine_2D.h` to understand vertex/triangle abstraction.
3. Move to `Engine_3D.h` and `Engine_4D.h` adapters.
4. Review `Engine_Matrix.h` conventions and transform generators.
5. Read `Engine_Graphics2DPipeline.h` and `Engine_Graphics3DPipeline.h` composition.
6. Finally step through `Main.cpp` demos.

This order mirrors the actual dependency stack and makes the codebase significantly easier to understand.
