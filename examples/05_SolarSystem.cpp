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
#include <iostream>
#include <thread>

// Renders a simple, solid 3D Diamond / Pyramid shape
void RenderPyramid(Engine::Graphics3DPipeline<double>& pipeline, double base_color) {
    // A single color scheme adapted using 5 shared vertices representing base and peak.
    
    std::vector<Engine::WorldVertex3D<double>> vertices = {
        Engine::WorldVertex3D<double>({-1, 0, -1}, std::max(1.0, base_color - 1)), // 0: back-left base
        Engine::WorldVertex3D<double>({ 1, 0, -1}, std::max(1.0, base_color - 1)), // 1: back-right base
        Engine::WorldVertex3D<double>({ 1, 0,  1}, std::min(9.0, base_color + 1)), // 2: front-right base
        Engine::WorldVertex3D<double>({-1, 0,  1}, std::min(9.0, base_color + 1)), // 3: front-left base
        Engine::WorldVertex3D<double>({ 0, 2,  0}, std::min(9.0, base_color + 3))  // 4: peak
    };

    std::vector<int> indices = {
        // Base (two triangles)
        0, 1, 2, 0, 2, 3,
        // Front face
        3, 2, 4,
        // Right face
        2, 1, 4,
        // Back face
        1, 0, 4,
        // Left face
        0, 3, 4
    };

    pipeline.RenderGeometry(vertices, indices);
}

int main() {
    TerminalBufferContext context(120, 30);
    Engine::ScalableInterpolator<double> color_interpolator;
    Engine::ContextGraphics<double> context_g(context);

    Engine::Graphics3DPipeline<double> pipeline(
        context_g, color_interpolator, Engine::Rectangle(0, 0, 120, 30));
    

    const double _deg_to_rad = std::acos(-1) / 180.0;
    
    // Set perspective
    pipeline.LoadIdentity();
    pipeline.Perspective(45 * _deg_to_rad, 120.0 / 30.0, 0.01, 100);
    pipeline.EnableDepthBuffer();

    for (int frame = 0; frame < 200; ++frame) {
        pipeline.ClearDepthBuffer();
        
        // Push the main camera transformation
        pipeline.PushMatrix();
        // Move camera out to observe solar system, view slightly top-down
        pipeline.Transform(
            Engine::Vector3(0, -1.0, -12),
            Engine::Vector3(20 * _deg_to_rad, 0, 0),
            Engine::Vector3(1, 1, 1)
        );

        // ======================================
        // --- CENTRAL SUN
        // ======================================
        pipeline.PushMatrix();
        // Rotate sun continuously on its Y axis
        pipeline.Rotate(Engine::Vector3(0, frame * 2 * _deg_to_rad, 0));
        // Center the pyramid pivot (offset y) and scale it bigger
        pipeline.Translate(Engine::Vector3(0, -1, 0));
        pipeline.Scale(Engine::Vector3(1.5, 1.5, 1.5));
        RenderPyramid(pipeline, 6); // Bright intensity 
        pipeline.PopMatrix();

        // ======================================
        // --- ORBITING PLANET
        // ======================================
        pipeline.PushMatrix();
        // The orbiting matrix translation: rotate first (defines orbit ring), then translate out
        pipeline.Rotate(Engine::Vector3(0, frame * -4 * _deg_to_rad, 0));
        pipeline.Translate(Engine::Vector3(5.0, 0, 0)); // Extend planet 5 units out from center
        
        // Planet's local rotation on its own axis
        pipeline.PushMatrix();
        pipeline.Rotate(Engine::Vector3(45 * _deg_to_rad, frame * -6 * _deg_to_rad, 45 * _deg_to_rad));
        pipeline.Translate(Engine::Vector3(0, -0.6, 0)); // Center it
        pipeline.Scale(Engine::Vector3(0.6, 0.6, 0.6));
        RenderPyramid(pipeline, 3); // Medium intensity
        pipeline.PopMatrix();

        // ======================================
        // --- ORBITING MOON (Orbiting the Planet)
        // ======================================
        // We are still within the Planet's local space coordinates
        pipeline.PushMatrix();
        // Moon's orbit around the planet
        pipeline.Rotate(Engine::Vector3(0, frame * 8 * _deg_to_rad, 0));
        pipeline.Translate(Engine::Vector3(1.8, 0, 0)); // Extend 1.8 units away from Planet
        
        // Moon's local transformation
        pipeline.Scale(Engine::Vector3(0.25, 0.25, 0.25));
        pipeline.Translate(Engine::Vector3(0, -1, 0));
        RenderPyramid(pipeline, 8); // Light colored Moon
        pipeline.PopMatrix(); // Pop Moon

        pipeline.PopMatrix(); // Pop Planet
        pipeline.PopMatrix(); // Pop Camera

        // Flush render output
        std::system("clear");
        context.Write();
        context.Clear();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    context.Report();
    return 0;
}
