/**
 * @file BoidsSettings.hpp
 * @brief Boids flocking algorithm settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

namespace libprojectM {
namespace Settings {

/**
 * @brief Boids behavior presets
 */
enum class BoidsPreset
{
    Custom,        //!< User-defined parameters
    Scattered,     //!< High separation, low cohesion (chaotic)
    Tight,         //!< High alignment and cohesion (coordinated)
    Clustered,     //!< Very high cohesion (dense swarms)
    Natural        //!< Balanced (realistic flocking)
};

/**
 * @brief Boids flocking settings
 */
struct PROJECTM_EXPORT BoidsSettings
{
    // Enable/Disable
    bool enabled{true};                  //!< Enable boids feature

    // Population
    int maxBoids{1000};                  //!< Maximum number of boids
    int spawnCount{500};                 //!< Initial spawn count

    // Behavior Weights
    float separationWeight{1.5f};        //!< Separation force weight
    float alignmentWeight{1.0f};         //!< Alignment force weight
    float cohesionWeight{1.0f};          //!< Cohesion force weight

    // Perception
    float perceptionRadius{3.0f};        //!< Neighbor detection radius
    float separationRadius{1.0f};        //!< Desired minimum spacing

    // Movement
    float maxSpeed{5.0f};                //!< Maximum velocity
    float maxForce{0.5f};                //!< Maximum steering force

    // Boundaries
    bool wrapBoundaries{true};           //!< Wrap at edges (toroidal space)
    float boundarySize{50.0f};           //!< Boundary box size

    // Audio Reactivity
    bool audioReactiveSpeed{true};       //!< Modulate speed with energy
    bool audioReactiveSeparation{true};  //!< Pulse separation with beats

    // Rendering
    float particleSize{2.0f};            //!< Particle rendering size
    bool connectNearby{false};           //!< Draw lines between nearby boids

    /**
     * @brief Apply behavior preset
     */
    void ApplyPreset(BoidsPreset preset)
    {
        switch (preset)
        {
            case BoidsPreset::Scattered:
                separationWeight = 2.0f;
                alignmentWeight = 0.5f;
                cohesionWeight = 0.5f;
                break;

            case BoidsPreset::Tight:
                separationWeight = 1.0f;
                alignmentWeight = 2.0f;
                cohesionWeight = 2.0f;
                break;

            case BoidsPreset::Clustered:
                separationWeight = 0.8f;
                alignmentWeight = 1.0f;
                cohesionWeight = 3.0f;
                break;

            case BoidsPreset::Natural:
                separationWeight = 1.5f;
                alignmentWeight = 1.0f;
                cohesionWeight = 1.0f;
                break;

            case BoidsPreset::Custom:
                // Keep current values
                break;
        }
    }

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = BoidsSettings{};
    }

    /**
     * @brief Validate and clamp values
     */
    void Validate()
    {
        // Clamp population
        if (maxBoids < 10) maxBoids = 10;
        if (maxBoids > 10000) maxBoids = 10000;

        if (spawnCount < 0) spawnCount = 0;
        if (spawnCount > maxBoids) spawnCount = maxBoids;

        // Clamp weights
        if (separationWeight < 0.0f) separationWeight = 0.0f;
        if (separationWeight > 10.0f) separationWeight = 10.0f;

        if (alignmentWeight < 0.0f) alignmentWeight = 0.0f;
        if (alignmentWeight > 10.0f) alignmentWeight = 10.0f;

        if (cohesionWeight < 0.0f) cohesionWeight = 0.0f;
        if (cohesionWeight > 10.0f) cohesionWeight = 10.0f;

        // Clamp radii
        if (perceptionRadius < 0.1f) perceptionRadius = 0.1f;
        if (perceptionRadius > 50.0f) perceptionRadius = 50.0f;

        if (separationRadius < 0.1f) separationRadius = 0.1f;
        if (separationRadius > perceptionRadius) separationRadius = perceptionRadius;

        // Clamp speeds
        if (maxSpeed < 0.1f) maxSpeed = 0.1f;
        if (maxSpeed > 50.0f) maxSpeed = 50.0f;

        if (maxForce < 0.01f) maxForce = 0.01f;
        if (maxForce > 10.0f) maxForce = 10.0f;

        // Clamp particle size
        if (particleSize < 0.5f) particleSize = 0.5f;
        if (particleSize > 20.0f) particleSize = 20.0f;
    }
};

} // namespace Settings
} // namespace libprojectM
