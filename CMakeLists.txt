cmake_minimum_required(VERSION 3.16)

project(GraphicsEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add_executable(GraphicsEngine
# 	src/Main.cpp
# 	src/Engine_Math.cpp
# )

# target_include_directories(GraphicsEngine PRIVATE include)
# Examples
add_executable(01_Pipeline2D examples/01_Pipeline2D.cpp src/Engine_Math.cpp)
target_include_directories(01_Pipeline2D PRIVATE include examples)

add_executable(02_Pipeline3D examples/02_Pipeline3D.cpp src/Engine_Math.cpp)
target_include_directories(02_Pipeline3D PRIVATE include examples)

add_executable(03_CustomPipeline3D examples/03_CustomPipeline3D.cpp src/Engine_Math.cpp)
target_include_directories(03_CustomPipeline3D PRIVATE include examples)

add_executable(04_DynamicTerrain examples/04_DynamicTerrain.cpp src/Engine_Math.cpp)
target_include_directories(04_DynamicTerrain PRIVATE include examples)

add_executable(05_SolarSystem examples/05_SolarSystem.cpp src/Engine_Math.cpp)
target_include_directories(05_SolarSystem PRIVATE include examples)

add_executable(06_TexturePipeline3D examples/06_TexturePipeline3D.cpp src/Engine_Math.cpp)
target_include_directories(06_TexturePipeline3D PRIVATE include examples)

enable_testing()

add_executable(GraphicsEngineTests
	tests/TestMain.cpp
	tests/GraphicsTests.cpp
	tests/MathMatrixTests.cpp
	tests/WorldGraphicsTests.cpp
	tests/PipelineTests.cpp
	src/Engine_Math.cpp
)
target_include_directories(GraphicsEngineTests PRIVATE include tests)

add_test(NAME GraphicsEngineTests COMMAND GraphicsEngineTests)