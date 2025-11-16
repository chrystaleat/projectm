/**
 * @file Hyperdimensional4DDemo.cpp
 * @brief Demo showcasing 4D Tesseract and hyperdimensional projection
 *
 * Visualizes a 4D hypercube rotating in hyperspace, projected
 * through 4D->3D->2D for mind-bending visualizations!
 *
 * Usage:
 *   - Create Tesseract (4D hypercube)
 *   - Rotate in 4D space
 *   - Project to 3D, then 2D
 *   - Render wireframe or points
 */

#include <Renderer/Tesseract.hpp>
#include <Renderer/Vec4D.hpp>
#include <Renderer/HyperdimensionalProjection.hpp>
#include <Audio/FrameAudioData.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <iomanip>
#include <numbers>

using namespace libprojectM::Renderer;
using namespace libprojectM::Audio;

/**
 * @brief 4D Hyperdimensional Visualization Demo
 *
 * Experience the fourth dimension!
 */
class Hyperdimensional4DDemo
{
public:
    Hyperdimensional4DDemo()
        : tesseract(2.0f)  // Side length 2
        , rotation4D_XY(0.0f)
        , rotation4D_ZW(0.0f)
    {
        std::cout << "=== 4D HYPERDIMENSIONAL DEMO ===" << std::endl;
        std::cout << "Creating tesseract (4D hypercube)..." << std::endl;

        const auto& vertices = tesseract.GetVertices();
        const auto& edges = tesseract.GetEdges();

        std::cout << "✓ Tesseract created!" << std::endl;
        std::cout << "  Vertices: " << vertices.size() << " (2^4 = 16)" << std::endl;
        std::cout << "  Edges: " << edges.size() << " (4 * 2^3 = 32)" << std::endl;
    }

    void DemoBasicRotation()
    {
        std::cout << "\n🔄 BASIC 4D ROTATION" << std::endl;
        std::cout << "Rotating tesseract in XY and ZW planes..." << std::endl;

        // Rotate slowly
        float angleXY = std::numbers::pi_v<float> / 4.0f;  // 45 degrees
        float angleZW = std::numbers::pi_v<float> / 6.0f;  // 30 degrees

        tesseract.Rotate(angleXY, angleZW);

        std::cout << "Rotation applied:" << std::endl;
        std::cout << "  XY plane: " << (angleXY * 180.0f / std::numbers::pi_v<float>) << "°" << std::endl;
        std::cout << "  ZW plane: " << (angleZW * 180.0f / std::numbers::pi_v<float>) << "°" << std::endl;

        ShowVertexPositions();
    }

    void DemoProjection()
    {
        std::cout << "\n📐 4D → 3D → 2D PROJECTION" << std::endl;

        // Setup projection parameters
        float cameraDistance4D = 8.0f;
        int screenWidth = 1920;
        int screenHeight = 1080;

        // Create 3D projection matrix (FOV 60°, aspect 16:9, near 0.1, far 100)
        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(60.0f),
            16.0f / 9.0f,
            0.1f,
            100.0f
        );

        std::cout << "Projection parameters:" << std::endl;
        std::cout << "  4D camera distance: " << cameraDistance4D << std::endl;
        std::cout << "  Screen resolution: " << screenWidth << "x" << screenHeight << std::endl;

        // Project all vertices
        auto screenPoints = ProjectTesseractTo2D(
            tesseract,
            cameraDistance4D,
            projectionMatrix,
            screenWidth,
            screenHeight
        );

        std::cout << "\n✓ Projected " << screenPoints.size() << " vertices to 2D!" << std::endl;

        // Show first few projected points
        std::cout << "\nFirst 5 screen positions:" << std::endl;
        for (size_t i = 0; i < 5 && i < screenPoints.size(); ++i)
        {
            std::cout << "  Vertex " << i << ": ("
                      << std::fixed << std::setprecision(1)
                      << screenPoints[i].x << ", "
                      << screenPoints[i].y << ")" << std::endl;
        }
    }

    void DemoAudioReactive()
    {
        std::cout << "\n🎵 AUDIO-REACTIVE 4D ROTATION" << std::endl;

        rotation4D_XY = 0.0f;
        rotation4D_ZW = 0.0f;

        // Simulate 60 frames with audio
        for (int frame = 0; frame < 60; ++frame)
        {
            // Simulate audio (BPM 120, beat every 30 frames)
            float bpm = 120.0f;
            float beatPhase = std::fmod(frame / 30.0f, 1.0f);
            float energy = 0.5f + 0.3f * std::sin(frame * 0.1f);

            // Rotate based on BPM
            float rotationSpeed = bpm / 60.0f * 0.05f;  // Radians per frame
            rotation4D_XY += rotationSpeed;
            rotation4D_ZW += rotationSpeed * 0.7f;  // Different speed

            // Scale with energy
            float scale = 0.8f + energy * 0.4f;

            // Apply rotation and scale
            tesseract.Reset();
            tesseract.Rotate(rotation4D_XY, rotation4D_ZW);
            tesseract.SetScale(scale);

            if (frame % 15 == 0)
            {
                std::cout << "Frame " << std::setw(2) << frame
                          << ": XY=" << std::fixed << std::setprecision(2)
                          << rotation4D_XY
                          << ", ZW=" << rotation4D_ZW
                          << ", Scale=" << scale << std::endl;
            }
        }

        std::cout << "✓ Audio-reactive rotation complete!" << std::endl;
    }

    void DemoDoubleRotation()
    {
        std::cout << "\n🌀 DOUBLE ROTATION (XY + ZW simultaneously)" << std::endl;
        std::cout << "This is a true 4D rotation that can't exist in 3D!" << std::endl;

        tesseract.Reset();

        // Both planes rotate at once
        float angle = std::numbers::pi_v<float> / 2.0f;  // 90 degrees
        tesseract.Rotate(angle, angle);

        std::cout << "Applied 90° rotation in BOTH XY and ZW planes" << std::endl;
        std::cout << "Result: Mind-bending 4D transformation!" << std::endl;

        ShowVertexPositions();
    }

    void DemoEdgeRendering()
    {
        std::cout << "\n📊 EDGE RENDERING GUIDE" << std::endl;

        const auto& edges = tesseract.GetEdges();
        const auto& vertices = tesseract.GetVertices();

        std::cout << "Tesseract has " << edges.size() << " edges to render" << std::endl;
        std::cout << "\nFirst 5 edges (vertex index pairs):" << std::endl;

        for (size_t i = 0; i < 5 && i < edges.size(); ++i)
        {
            const auto& edge = edges[i];
            const auto& v1 = vertices[edge.first];
            const auto& v2 = vertices[edge.second];

            std::cout << "  Edge " << i << ": "
                      << edge.first << " → " << edge.second
                      << " | Length: " << std::fixed << std::setprecision(2)
                      << CalculateDistance(v1, v2) << std::endl;
        }

        std::cout << "\nRendering pseudocode:" << std::endl;
        std::cout << "  for edge in tesseract.GetEdges():" << std::endl;
        std::cout << "    v1_4d = vertices[edge.first]" << std::endl;
        std::cout << "    v2_4d = vertices[edge.second]" << std::endl;
        std::cout << "    v1_2d = project(v1_4d)" << std::endl;
        std::cout << "    v2_2d = project(v2_4d)" << std::endl;
        std::cout << "    drawLine(v1_2d, v2_2d)" << std::endl;
    }

private:
    Tesseract tesseract;
    float rotation4D_XY;
    float rotation4D_ZW;

    void ShowVertexPositions()
    {
        const auto& vertices = tesseract.GetVertices();

        std::cout << "\nVertex positions (first 4):" << std::endl;
        for (size_t i = 0; i < 4 && i < vertices.size(); ++i)
        {
            const auto& v = vertices[i];
            std::cout << "  V" << i << ": ("
                      << std::fixed << std::setprecision(2)
                      << v.x << ", " << v.y << ", "
                      << v.z << ", " << v.w << ")" << std::endl;
        }
    }

    float CalculateDistance(const Vec4D& v1, const Vec4D& v2)
    {
        Vec4D diff = v1 - v2;
        return diff.Length();
    }
};

/**
 * @brief Main demo entry point
 */
int main()
{
    Hyperdimensional4DDemo demo;

    demo.DemoBasicRotation();
    demo.DemoProjection();
    demo.DemoDoubleRotation();
    demo.DemoAudioReactive();
    demo.DemoEdgeRendering();

    std::cout << "\n=== VISUALIZATION TIPS ===" << std::endl;
    std::cout << "\nRendering Modes:" << std::endl;
    std::cout << "  1. WIREFRAME: Draw all 32 edges as lines" << std::endl;
    std::cout << "  2. VERTICES: Render 16 vertices as glowing spheres" << std::endl;
    std::cout << "  3. FACES: Extract 2D faces and fill with color" << std::endl;
    std::cout << "  4. HYBRID: Wireframe + vertex highlights" << std::endl;

    std::cout << "\nColor Schemes:" << std::endl;
    std::cout << "  - W-axis gradient: Color based on 4th dimension position" << std::endl;
    std::cout << "  - Depth-based: Fade edges far from 4D camera" << std::endl;
    std::cout << "  - Audio-reactive: HSV hue from bass/mid/treble" << std::endl;

    std::cout << "\nRotation Presets:" << std::endl;
    std::cout << "  🎵 Beat-sync: Rotate XY with BPM, ZW with half-BPM" << std::endl;
    std::cout << "  🌊 Slow drift: Small constant rotation for hypnotic effect" << std::endl;
    std::cout << "  ⚡ Energy burst: Fast rotation spikes on high energy" << std::endl;

    std::cout << "\n✓ Demo complete! Welcome to the 4th dimension! 🌀" << std::endl;

    return 0;
}
