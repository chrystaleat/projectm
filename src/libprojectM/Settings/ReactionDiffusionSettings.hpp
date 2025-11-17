/**
 * @file ReactionDiffusionSettings.hpp
 * @brief Reaction-Diffusion pattern generator settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

namespace libprojectM {
namespace Settings {

/**
 * @brief Reaction-Diffusion pattern presets
 */
enum class RDPreset
{
    Custom,        //!< User-defined parameters
    Spots,         //!< Leopard spots (F=0.055, k=0.062)
    Stripes,       //!< Zebra stripes (F=0.035, k=0.065)
    Spirals,       //!< Spiral waves (F=0.014, k=0.054)
    Chaos,         //!< Chaotic patterns (F=0.062, k=0.061)
    Waves          //!< Wave fronts (F=0.020, k=0.050)
};

/**
 * @brief Reaction-Diffusion settings
 */
struct PROJECTM_EXPORT ReactionDiffusionSettings
{
    // Enable/Disable
    bool enabled{true};                  //!< Enable reaction-diffusion feature

    // Grid Size
    int gridWidth{128};                  //!< Grid width (power of 2)
    int gridHeight{128};                 //!< Grid height (power of 2)

    // Pattern
    RDPreset preset{RDPreset::Spots};    //!< Pattern preset

    // Gray-Scott Parameters (when using Custom preset)
    float feedRate{0.055f};              //!< F parameter (feed rate)
    float killRate{0.062f};              //!< k parameter (kill rate)
    float diffusionU{0.16f};             //!< Du (U diffusion rate)
    float diffusionV{0.08f};             //!< Dv (V diffusion rate)

    // Seeding
    int seedCount{10};                   //!< Number of initial seed points
    int seedRadius{5};                   //!< Radius of each seed
    bool reseedPeriodically{false};      //!< Reseed every N seconds
    int reseedInterval{30};              //!< Reseed interval in seconds

    // Audio Reactivity
    bool audioReactiveFeed{true};        //!< Modulate feed rate with energy
    bool audioReactiveKill{true};        //!< Modulate kill rate with beats

    // Rendering
    bool wrapBoundaries{true};           //!< Wrap boundaries (toroidal)
    float opacity{1.0f};                 //!< Rendering opacity

    /**
     * @brief Apply pattern preset
     */
    void ApplyPreset(RDPreset presetType)
    {
        preset = presetType;

        switch (presetType)
        {
            case RDPreset::Spots:
                feedRate = 0.055f;
                killRate = 0.062f;
                diffusionU = 0.16f;
                diffusionV = 0.08f;
                break;

            case RDPreset::Stripes:
                feedRate = 0.035f;
                killRate = 0.065f;
                diffusionU = 0.16f;
                diffusionV = 0.08f;
                break;

            case RDPreset::Spirals:
                feedRate = 0.014f;
                killRate = 0.054f;
                diffusionU = 0.16f;
                diffusionV = 0.08f;
                break;

            case RDPreset::Chaos:
                feedRate = 0.062f;
                killRate = 0.061f;
                diffusionU = 0.16f;
                diffusionV = 0.08f;
                break;

            case RDPreset::Waves:
                feedRate = 0.020f;
                killRate = 0.050f;
                diffusionU = 0.14f;
                diffusionV = 0.06f;
                break;

            case RDPreset::Custom:
                // Keep current values
                break;
        }
    }

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = ReactionDiffusionSettings{};
    }

    /**
     * @brief Validate and clamp values
     */
    void Validate()
    {
        // Ensure grid size is power of 2
        auto roundToPowerOfTwo = [](int value) {
            int result = 1;
            while (result < value && result < 2048)
            {
                result *= 2;
            }
            return result;
        };

        gridWidth = roundToPowerOfTwo(gridWidth);
        gridHeight = roundToPowerOfTwo(gridHeight);

        // Clamp grid size
        if (gridWidth < 32) gridWidth = 32;
        if (gridWidth > 1024) gridWidth = 1024;
        if (gridHeight < 32) gridHeight = 32;
        if (gridHeight > 1024) gridHeight = 1024;

        // Clamp parameters
        if (feedRate < 0.01f) feedRate = 0.01f;
        if (feedRate > 0.1f) feedRate = 0.1f;

        if (killRate < 0.03f) killRate = 0.03f;
        if (killRate > 0.08f) killRate = 0.08f;

        if (diffusionU < 0.01f) diffusionU = 0.01f;
        if (diffusionU > 0.5f) diffusionU = 0.5f;

        if (diffusionV < 0.01f) diffusionV = 0.01f;
        if (diffusionV > 0.5f) diffusionV = 0.5f;

        // Clamp seeding
        if (seedCount < 1) seedCount = 1;
        if (seedCount > 100) seedCount = 100;

        if (seedRadius < 1) seedRadius = 1;
        if (seedRadius > 50) seedRadius = 50;

        // Clamp opacity
        if (opacity < 0.0f) opacity = 0.0f;
        if (opacity > 1.0f) opacity = 1.0f;
    }
};

} // namespace Settings
} // namespace libprojectM
