/**
 * @file JuliaSetSettings.hpp
 * @brief Quaternion Julia Set fractal settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

namespace libprojectM {
namespace Settings {

/**
 * @brief Julia set fractal presets
 */
enum class JuliaPreset
{
    Custom,        //!< User-defined c parameter
    Classic,       //!< Classic Julia set
    Spiral,        //!< Spiral structures
    Dragon,        //!< Dragon-like shapes
    Bouquet,       //!< Flower-like formations
    Chaotic,       //!< Chaotic patterns
    Symmetrical    //!< 4-fold symmetry
};

/**
 * @brief Quaternion Julia Set settings
 */
struct PROJECTM_EXPORT JuliaSetSettings
{
    // Enable/Disable
    bool enabled{true};                  //!< Enable Julia set feature

    // Pattern
    JuliaPreset preset{JuliaPreset::Classic};  //!< Fractal preset

    // Custom c parameter (when using Custom preset)
    float c_a{-0.8f};                    //!< c.a (real part)
    float c_b{0.156f};                   //!< c.b (i component)
    float c_c{0.0f};                     //!< c.c (j component)
    float c_d{0.0f};                     //!< c.d (k component)

    // Quality
    int sampleCount{5000};               //!< Number of samples
    int maxIterations{100};              //!< Maximum iterations per point
    float escapeRadius{2.0f};            //!< Escape threshold

    // Rendering
    float samplingBounds{2.0f};          //!< 4D sampling bounds
    float renderSize{1.0f};              //!< Point cloud size multiplier
    float pointSize{3.0f};               //!< Individual point size

    // Update Frequency
    bool regenerateContinuously{false};  //!< Regenerate every frame (expensive!)
    int regenerateInterval{60};          //!< Regenerate every N frames

    // Audio Reactivity
    bool audioReactiveMorph{true};       //!< Morph c parameter with beats
    bool audioReactiveSize{true};        //!< Scale with energy

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = JuliaSetSettings{};
    }

    /**
     * @brief Validate and clamp values
     */
    void Validate()
    {
        // Clamp sample count
        if (sampleCount < 100) sampleCount = 100;
        if (sampleCount > 100000) sampleCount = 100000;

        // Clamp iterations
        if (maxIterations < 10) maxIterations = 10;
        if (maxIterations > 500) maxIterations = 500;

        // Clamp escape radius
        if (escapeRadius < 1.5f) escapeRadius = 1.5f;
        if (escapeRadius > 10.0f) escapeRadius = 10.0f;

        // Clamp bounds
        if (samplingBounds < 0.5f) samplingBounds = 0.5f;
        if (samplingBounds > 10.0f) samplingBounds = 10.0f;

        // Clamp sizes
        if (renderSize < 0.1f) renderSize = 0.1f;
        if (renderSize > 5.0f) renderSize = 5.0f;

        if (pointSize < 0.5f) pointSize = 0.5f;
        if (pointSize > 20.0f) pointSize = 20.0f;

        // Clamp regenerate interval
        if (regenerateInterval < 1) regenerateInterval = 1;
        if (regenerateInterval > 600) regenerateInterval = 600;

        // Clamp c parameter components
        auto clampC = [](float& value) {
            if (value < -2.0f) value = -2.0f;
            if (value > 2.0f) value = 2.0f;
        };

        clampC(c_a);
        clampC(c_b);
        clampC(c_c);
        clampC(c_d);
    }
};

} // namespace Settings
} // namespace libprojectM
