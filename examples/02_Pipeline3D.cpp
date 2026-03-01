#include "Engine/Engine.h"
#include "Engine/Engine_3D.h"
#include "Engine/Engine_Geometry.h"
#include "Engine/Engine_Graphics.h"
#include "Engine/Engine_Graphics3DPipeline.h"
#include "Engine/Engine_Interpolation.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

void RenderCube_pipe(Engine::Graphics3DPipeline<double> &pipeline) {
    // Helper function mapping 12 triangles forming a simple local cube at zero-point.
    
    std::vector<Engine::Vertex3D<double>> vertices = {
        {-1, -1, -1, 3}, // 0: bottom-left-back
        { 1, -1, -1, 4}, // 1: bottom-right-back
        { 1,  1, -1, 5}, // 2: top-right-back
        {-1,  1, -1, 6}, // 3: top-left-back
        {-1, -1,  1, 4}, // 4: bottom-left-front
        { 1, -1,  1, 5}, // 5: bottom-right-front
        { 1,  1,  1, 9}, // 6: top-right-front
        {-1,  1,  1, 8}  // 7: top-left-front
    };

    std::vector<int> indices = {
        4, 5, 6, 4, 6, 7, // Front
        1, 0, 3, 1, 3, 2, // Back
        0, 4, 7, 0, 7, 3, // Left
        5, 1, 2, 5, 2, 6, // Right
        3, 7, 6, 3, 6, 2, // Top
        0, 1, 5, 0, 5, 4  // Bottom
    };

    pipeline.RenderGeometry(vertices, indices);
}

int main() {
    TerminalBufferContext context(120, 30);
    Engine::Interpolator<double> color_interpolator;
    Engine::ContextGraphics<double> context_g(context);

    // Setup 3D graphics pipeline inside bounding rect
    Engine::Graphics3DPipeline<double> pipeline3d(
        context_g, color_interpolator, Engine::Rectangle(0, 0, 120, 30));

    // Prepare global logic bindings
    const double _deg_to_rad = std::acos(-1) / 180.0;
    
    // Define viewpoint base metrics
    pipeline3d.LoadIdentity();
    pipeline3d.Perspective(45 * _deg_to_rad, 120.0 / 30.0, 0.01, 1000);
    pipeline3d.EnableDepthBuffer();

    // Main interaction simulation
    for (int i = -100; i <= 100; ++i) {
        pipeline3d.ClearDepthBuffer();
        
        // Cube #1 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform3D(
            Engine::Vector3(-2, 0, -8),
            Engine::Vector3(0, (5 * i) * _deg_to_rad, 0), // Rotate continuously on Y axis
            Engine::Vector3(1, 0.5, 1)                      // Scale Y to flat block shape
        );
        RenderCube_pipe(pipeline3d);
        pipeline3d.PopMatrix();

        // Cube #2 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform3D(
            Engine::Vector3(0.08 * i, 0, -5),             // Move dynamically horizontally
            Engine::Vector3(1, 1, 0) * (3.14 * 0.025 * i), // Complex combined rotation
            Engine::Vector3(1, 0.5, 1)
        );
        RenderCube_pipe(pipeline3d);
        pipeline3d.PopMatrix();

        // Flush text display sequence to user
        std::system("clear");
        context.Write();
        context.Clear(); // reset backend buffer
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    context.Report();
    return 0;
}
