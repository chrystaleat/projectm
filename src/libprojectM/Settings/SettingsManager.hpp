/**
 * @file SettingsManager.hpp
 * @brief Settings manager for advanced projectM features
 *
 * Manages settings for advanced visualization features like boids, reaction-diffusion,
 * 4D visualization, and Julia sets. Works alongside the core projectM configuration.
 */

#pragma once

#include <projectM-4/projectM_export.h>

#include <functional>
#include <string>
#include <memory>

namespace libprojectM {
namespace Settings {

/**
 * @brief Categories of settings
 */
enum class SettingsCategory
{
    General,
    Audio,
    Boids,
    ReactionDiffusion,
    Hyperdimensional4D,
    JuliaSet
};

/**
 * @brief Boids flocking behavior settings
 */
struct BoidsSettings
{
    // Population
    int maxBoids = 100;
    int spawnCount = 50;

    // Behavior weights
    float separationWeight = 1.5f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 1.0f;

    // Movement
    float maxSpeed = 2.0f;
    float maxForce = 0.05f;

    // Perception
    float separationRadius = 25.0f;
    float alignmentRadius = 50.0f;
    float cohesionRadius = 50.0f;

    // Audio reactivity
    bool audioReactiveSeparation = true;
    bool audioReactiveSpeed = true;
    bool audioReactiveSpawn = false;
};

/**
 * @brief Reaction-Diffusion pattern settings
 */
struct ReactionDiffusionSettings
{
    // Grid
    int gridWidth = 256;
    int gridHeight = 256;

    // Gray-Scott parameters
    float feedRate = 0.055f;       // F
    float killRate = 0.062f;       // k
    float diffusionU = 1.0f;       // Du
    float diffusionV = 0.5f;       // Dv

    // Simulation
    float timestep = 1.0f;
    int iterationsPerFrame = 10;
    bool autoSeed = true;

    // Audio reactivity
    bool audioReactiveFeedRate = true;
    bool audioReactiveKillRate = false;
    bool audioReactiveDiffusion = false;
};

/**
 * @brief 4D hyperdimensional visualization settings
 */
struct Hyperdimensional4DSettings
{
    // Tesseract properties
    float tesseractSize = 2.0f;
    float edgeThickness = 2.0f;
    int renderMode = 0; // 0=wireframe, 1=vertices, 2=hybrid, 3=faces

    // 4D camera
    float camera4DDistance = 5.0f;
    float camera4DFOV = 45.0f;

    // Rotation speeds (radians/sec) for different 4D planes
    float rotationXY = 0.5f;
    float rotationZW = 0.3f;
    float rotationXZ = 0.2f;
    float rotationYW = 0.4f;

    // Rendering
    float vertexSize = 5.0f;
    float colorCycleSpeed = 0.5f;
    bool perspectiveProjection = true;

    // Audio reactivity
    bool audioReactiveRotation = true;
};

/**
 * @brief Julia set fractal settings
 */
struct JuliaSetSettings
{
    // Julia set quaternion parameter c = (a + bi + cj + dk)
    float c_a = -0.2f;
    float c_b = 0.65f;
    float c_c = -0.3f;
    float c_d = 0.4f;

    // Quality
    int maxIterations = 50;
    int sampleCount = 100000;

    // Rendering
    float escapeRadius = 2.0f;
    float pointSize = 2.0f;

    // Bounds
    float minBound = -2.0f;
    float maxBound = 2.0f;

    // Animation
    float cParameterSpeed = 0.1f;
    bool autoRotate = true;

    // Audio reactivity
    bool audioReactiveCParameter = true;
    bool audioReactiveIterations = false;
};

/**
 * @brief Main settings manager class
 */
class PROJECTM_EXPORT SettingsManager
{
public:
    SettingsManager();
    ~SettingsManager();

    // Get settings references
    BoidsSettings& GetBoidsSettings() { return m_boidsSettings; }
    ReactionDiffusionSettings& GetReactionDiffusionSettings() { return m_rdSettings; }
    Hyperdimensional4DSettings& GetHyperdimensional4DSettings() { return m_4dSettings; }
    JuliaSetSettings& GetJuliaSetSettings() { return m_juliaSetSettings; }

    // Const getters
    const BoidsSettings& GetBoidsSettings() const { return m_boidsSettings; }
    const ReactionDiffusionSettings& GetReactionDiffusionSettings() const { return m_rdSettings; }
    const Hyperdimensional4DSettings& GetHyperdimensional4DSettings() const { return m_4dSettings; }
    const JuliaSetSettings& GetJuliaSetSettings() const { return m_juliaSetSettings; }

    // Change notification
    using ChangeCallback = std::function<void(SettingsCategory)>;
    void RegisterChangeCallback(ChangeCallback callback);
    void NotifyChange(SettingsCategory category);

    // Persistence
    bool SaveToFile(const std::string& filepath);
    bool LoadFromFile(const std::string& filepath);

    // Reset to defaults
    void ResetToDefaults(SettingsCategory category);
    void ResetAll();

private:
    BoidsSettings m_boidsSettings;
    ReactionDiffusionSettings m_rdSettings;
    Hyperdimensional4DSettings m_4dSettings;
    JuliaSetSettings m_juliaSetSettings;

    ChangeCallback m_changeCallback;
};

} // namespace Settings
} // namespace libprojectM
