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

// Procedurally generates a grid-based terrain that animates over time.
// This creates a cool "synthwave" or "ocean" rolling wave effect mathematically.
void RenderTerrain(Engine::Graphics3DPipeline<double>& pipeline, double time_offset) {
    const int GRID_SIZE = 14; 
    const double SPACING = 0.6; // Space between each vertex
    
    std::vector<Engine::Vertex3D<double>> vertices;
    vertices.reserve(GRID_SIZE * GRID_SIZE);
    
    std::vector<int> indices;
    indices.reserve((GRID_SIZE - 1) * (GRID_SIZE - 1) * 6);
    
    // Mathematical height function (sine/cosine waves mapped by procedural time)
    auto getHeight = [time_offset](double vx, double vz) {
        // Combine sine and cosine to create organic rolling hills
        return std::sin(vx * 1.5 + time_offset * 2.0) * 0.4 + 
               std::cos(vz * 1.5 + time_offset * 1.5) * 0.4;
    };
    
    // Generate a color index (1 to 9) based on the height of the terrain
    auto getColor = [](double y) {
        return std::max(1.0, std::min(9.0, 3.0 + (y + 0.8) * 4.0));
    };

    // Pre-calculate all vertices so they can be natively shared among triangle edges
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int z = 0; z < GRID_SIZE; ++z) {
            double vx = (x - GRID_SIZE / 2.0) * SPACING;
            double vz = (z - GRID_SIZE / 2.0) * SPACING;
            double vy = getHeight(vx, vz);
            vertices.push_back(Engine::Vertex3D<double>(vx, vy, vz, getColor(vy)));
        }
    }
    
    // Wire up pairs of triangles indexing correctly across the array elements
    for (int x = 0; x < GRID_SIZE - 1; ++x) {
        for (int z = 0; z < GRID_SIZE - 1; ++z) {
            int i00 = x * GRID_SIZE + z;
            int i01 = x * GRID_SIZE + (z + 1);
            int i10 = (x + 1) * GRID_SIZE + z;
            int i11 = (x + 1) * GRID_SIZE + (z + 1);
            
            // Triangle 1 (Bottom-left to Top-Right split)
            indices.push_back(i00);
            indices.push_back(i10);
            indices.push_back(i01);
            
            // Triangle 2
            indices.push_back(i10);
            indices.push_back(i11);
            indices.push_back(i01);
        }
    }
    
    pipeline.RenderGeometry(vertices, indices);
}

int main() {
    TerminalBufferContext context(120, 30);
    Engine::Interpolator<double> color_interpolator;
    Engine::ContextGraphics<double> context_g(context);

    Engine::Graphics3DPipeline<double> pipeline3d(
        context_g, color_interpolator, Engine::Rectangle(0, 0, 120, 30));

    const double _deg_to_rad = std::acos(-1) / 180.0;
    
    pipeline3d.LoadIdentity();
    pipeline3d.Perspective(55 * _deg_to_rad, 120.0 / 30.0, 0.01, 100);
    pipeline3d.EnableDepthBuffer();

    double time = 0.0;
    
    // Animate across 200 frames
    for (int i = 0; i < 200; ++i) {
        pipeline3d.ClearDepthBuffer();
        
        pipeline3d.PushMatrix();
        
        // Tilt the camera down 30 degrees and move it back/up slightly to see the terrain
        pipeline3d.Transform3D(
            Engine::Vector3(0, -2.5, -8),
            Engine::Vector3(30 * _deg_to_rad, 0, 0),
            Engine::Vector3(1, 1, 1)
        );
        
        // Slowly rotate the entire terrain while moving the wave phase
        pipeline3d.Rotate3D(Engine::Vector3(0, time * 0.2, 0));

        RenderTerrain(pipeline3d, time);
        
        pipeline3d.PopMatrix();

        std::system("clear");
        context.Write();
        context.Clear();
        
        time += 0.15; // advance time offset
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    context.Report();
    return 0;
}
