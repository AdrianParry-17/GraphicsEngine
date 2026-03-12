#include "Engine/Engine.h"
#include "common/TerminalBufferContext.h"
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

void RenderCube_pipe(Engine::Graphics3DPipeline<double> &pipeline) {
    // Helper function mapping 12 triangles forming a simple local cube at zero-point.
    
    std::vector<Engine::WorldVertex3D<double>> vertices = {
        // Front face (Z = 1), Color: 9
        Engine::WorldVertex3D<double>({-1, -1,  1}, 9),
        Engine::WorldVertex3D<double>({ 1, -1,  1}, 9),
        Engine::WorldVertex3D<double>({ 1,  1,  1}, 9),
        Engine::WorldVertex3D<double>({-1,  1,  1}, 9),
        // Back face (Z = -1), Color: 4
        Engine::WorldVertex3D<double>({-1, -1, -1}, 4),
        Engine::WorldVertex3D<double>({ 1, -1, -1}, 4),
        Engine::WorldVertex3D<double>({ 1,  1, -1}, 4),
        Engine::WorldVertex3D<double>({-1,  1, -1}, 4),
        // Left face (X = -1), Color: 5
        Engine::WorldVertex3D<double>({-1, -1, -1}, 5),
        Engine::WorldVertex3D<double>({-1, -1,  1}, 5),
        Engine::WorldVertex3D<double>({-1,  1,  1}, 5),
        Engine::WorldVertex3D<double>({-1,  1, -1}, 5),
        // Right face (X = 1), Color: 6
        Engine::WorldVertex3D<double>({ 1, -1, -1}, 6),
        Engine::WorldVertex3D<double>({ 1, -1,  1}, 6),
        Engine::WorldVertex3D<double>({ 1,  1,  1}, 6),
        Engine::WorldVertex3D<double>({ 1,  1, -1}, 6),
        // Top face (Y = 1), Color: 7
        Engine::WorldVertex3D<double>({-1,  1, -1}, 7),
        Engine::WorldVertex3D<double>({ 1,  1, -1}, 7),
        Engine::WorldVertex3D<double>({ 1,  1,  1}, 7),
        Engine::WorldVertex3D<double>({-1,  1,  1}, 7),
        // Bottom face (Y = -1), Color: 8
        Engine::WorldVertex3D<double>({-1, -1, -1}, 8),
        Engine::WorldVertex3D<double>({ 1, -1, -1}, 8),
        Engine::WorldVertex3D<double>({ 1, -1,  1}, 8),
        Engine::WorldVertex3D<double>({-1, -1,  1}, 8)
    };

    std::vector<int> indices = {
        0,  1,  2,  0,  2,  3,  // Front
        5,  4,  7,  5,  7,  6,  // Back
        8,  9, 10,  8, 10, 11,  // Left
        13, 12, 15, 13, 15, 14, // Right
        16, 19, 18, 16, 18, 17, // Top
        20, 21, 22, 20, 22, 23  // Bottom
    };

    pipeline.RenderGeometry(vertices, indices);
}

int main() {
    TerminalBufferContext context(120, 30);
    Engine::ScalableInterpolator<double> color_interpolator;
    Engine::ContextGraphics<double> context_g(context);

    // Setup 3D graphics pipeline inside bounding rect
    Engine::Graphics3DPipeline<double> pipeline3d(
        context_g, color_interpolator, Engine::Rectangle(0, 0, 120, 30));

    // Prepare global logic bindings
    const double _deg_to_rad = std::acos(-1) / 180.0;
    
    pipeline3d.EnablePerspectiveCorrection();
    
    // Define viewpoint base metrics
    pipeline3d.LoadIdentity();
    pipeline3d.Perspective(45 * _deg_to_rad, 120.0 / 30.0, 0.01, 1000);
    pipeline3d.EnableDepthBuffer();

    // Main interaction simulation
    for (int i = -100; i <= 100; ++i) {
        pipeline3d.ClearDepthBuffer();
        
        // Cube #1 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
            Engine::Vector3(-2, 0, -8),
            Engine::Vector3(0, (5 * i) * _deg_to_rad, 0), // Rotate continuously on Y axis
            Engine::Vector3(1, 0.5, 1)                      // Scale Y to flat block shape
        );
        RenderCube_pipe(pipeline3d);
        pipeline3d.PopMatrix();

        // Cube #2 Generation
        pipeline3d.PushMatrix();
        pipeline3d.Transform(
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
