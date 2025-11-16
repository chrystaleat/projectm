/**
 * @file BoidsDemo.cpp
 * @brief Demo showcasing the Boids flocking algorithm
 *
 * This demo creates a beautiful swarm of boids that flock together
 * in response to audio. Perfect for ambient, flowing visualizations!
 *
 * Usage:
 *   - Create BoidsSystem
 *   - Spawn boids
 *   - Update with audio data
 *   - Render as point cloud or connected mesh
 */

#include <Renderer/BoidsSystem.hpp>
#include <Renderer/VerletParticle.hpp>
#include <Audio/FrameAudioData.hpp>

#include <iostream>
#include <vector>

using namespace libprojectM::Renderer;
using namespace libprojectM::Audio;

/**
 * @brief Boids Flocking Demo
 *
 * Creates an organic swarm that responds to music!
 */
class BoidsFlockingDemo
{
public:
    BoidsFlockingDemo()
        : boidsSystem(1000)  // Max 1000 boids
    {
        InitializeBoids();
    }

    void InitializeBoids()
    {
        std::cout << "=== BOIDS FLOCKING DEMO ===" << std::endl;
        std::cout << "Creating swarm of 500 boids..." << std::endl;

        // Configure flocking behavior
        boidsSystem.SetSeparationWeight(1.5f);  // Strong avoidance
        boidsSystem.SetAlignmentWeight(1.0f);   // Moderate velocity matching
        boidsSystem.SetCohesionWeight(1.0f);    // Moderate group attraction

        boidsSystem.SetPerceptionRadius(3.0f);  // See neighbors within 3 units
        boidsSystem.SetMaxSpeed(5.0f);          // Speed limit
        boidsSystem.SetMaxForce(0.5f);          // Steering force limit

        // Set bounds (wrap mode = toroidal space)
        boidsSystem.SetBoundingBox(-50.0f, -50.0f, -50.0f,
                                   50.0f, 50.0f, 50.0f);
        boidsSystem.SetBoundaryMode(BoundaryMode::Wrap);

        // Spawn boids in a scattered formation
        for (int i = 0; i < 500; ++i)
        {
            float x = (rand() % 100 - 50) * 0.5f;
            float y = (rand() % 100 - 50) * 0.5f;
            float z = (rand() % 100 - 50) * 0.5f;

            float vx = (rand() % 100 - 50) * 0.01f;
            float vy = (rand() % 100 - 50) * 0.01f;
            float vz = (rand() % 100 - 50) * 0.01f;

            boidsSystem.AddBoid(glm::vec3(x, y, z), glm::vec3(vx, vy, vz));
        }

        std::cout << "✓ " << boidsSystem.GetBoidCount() << " boids spawned!" << std::endl;
    }

    void UpdateWithAudio(const FrameAudioData& audioData, float deltaTime)
    {
        // Calculate audio features
        float energy = CalculateEnergy(audioData);
        float bass = audioData.bass;
        float beat = DetectBeat(audioData);

        // Audio-reactive parameters
        boidsSystem.SetAudioEnergyMultiplier(1.0f + energy * 0.5f);  // Speed boost
        boidsSystem.SetBeatIntensity(beat);                           // Separation pulse

        // Update simulation
        boidsSystem.Update(deltaTime);
    }

    void Render()
    {
        const auto& boids = boidsSystem.GetBoids();

        std::cout << "\n=== RENDERING BOIDS ===" << std::endl;
        std::cout << "Active boids: " << boidsSystem.GetBoidCount() << std::endl;

        // Example: Render as point cloud
        std::cout << "\nPoint cloud positions:" << std::endl;
        int count = 0;
        for (const auto* boid : boids)
        {
            if (boid && boid->active && count < 10)  // Show first 10
            {
                std::cout << "  Boid " << count << ": ("
                          << boid->position.x << ", "
                          << boid->position.y << ", "
                          << boid->position.z << ")" << std::endl;
                count++;
            }
        }

        std::cout << "  ... and " << (boidsSystem.GetBoidCount() - 10) << " more" << std::endl;

        // In a real renderer, you would:
        // 1. Upload boid positions to GPU
        // 2. Render as GL_POINTS with point sprites
        // 3. Or connect nearby boids with lines
        // 4. Or render as instanced meshes
    }

    // Preset configurations
    void SetPresetScattered()
    {
        std::cout << "\n🔸 PRESET: Scattered (chaotic)" << std::endl;
        boidsSystem.SetSeparationWeight(2.0f);
        boidsSystem.SetAlignmentWeight(0.5f);
        boidsSystem.SetCohesionWeight(0.5f);
    }

    void SetPresetTight()
    {
        std::cout << "\n🔹 PRESET: Tight Formation (coordinated)" << std::endl;
        boidsSystem.SetSeparationWeight(1.0f);
        boidsSystem.SetAlignmentWeight(2.0f);
        boidsSystem.SetCohesionWeight(2.0f);
    }

    void SetPresetClustered()
    {
        std::cout << "\n🔺 PRESET: Clustered (swarm)" << std::endl;
        boidsSystem.SetSeparationWeight(0.8f);
        boidsSystem.SetAlignmentWeight(1.0f);
        boidsSystem.SetCohesionWeight(3.0f);
    }

private:
    BoidsSystem boidsSystem;

    float CalculateEnergy(const FrameAudioData& audioData)
    {
        // Simple energy calculation
        float sum = 0.0f;
        for (const auto& val : audioData.waveformLeft)
        {
            sum += std::abs(val);
        }
        return std::min(1.0f, sum / audioData.waveformLeft.size() * 10.0f);
    }

    float DetectBeat(const FrameAudioData& audioData)
    {
        // Simple beat detection (bass + treble spike)
        return (audioData.bass > 1.5f || audioData.treb > 1.5f) ? 1.0f : 0.0f;
    }
};

/**
 * @brief Main demo entry point
 */
int main()
{
    BoidsFlockingDemo demo;

    std::cout << "\n=== SIMULATING 60 FRAMES ===" << std::endl;

    // Simulate 60 frames
    for (int frame = 0; frame < 60; ++frame)
    {
        // Create dummy audio data
        FrameAudioData audioData;
        audioData.bass = 1.0f + 0.5f * std::sin(frame * 0.1f);
        audioData.mid = 1.0f;
        audioData.treb = 1.0f + 0.3f * std::sin(frame * 0.15f);

        // Update
        demo.UpdateWithAudio(audioData, 0.016f);

        // Show preset changes
        if (frame == 20)
        {
            demo.SetPresetScattered();
        }
        else if (frame == 40)
        {
            demo.SetPresetTight();
        }
    }

    // Final render
    demo.Render();

    std::cout << "\n✓ Demo complete!" << std::endl;
    std::cout << "\nIntegration notes:" << std::endl;
    std::cout << "  - Upload boid positions to GPU buffer" << std::endl;
    std::cout << "  - Render as instanced particles or point sprites" << std::endl;
    std::cout << "  - Color based on velocity magnitude" << std::endl;
    std::cout << "  - Connect nearby boids with lines for web effect" << std::endl;

    return 0;
}
