/**
 * @file AudioSettings.hpp
 * @brief Audio processing and reactivity settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

namespace libprojectM {
namespace Settings {

/**
 * @brief Audio processing settings
 */
struct PROJECTM_EXPORT AudioSettings
{
    // Beat Detection
    bool enableBeatDetection{true};      //!< Enable beat detection
    float beatSensitivity{1.0f};         //!< Beat sensitivity (0.1 - 5.0)
    float bassThreshold{1.2f};           //!< Bass beat threshold
    float trebleThreshold{1.2f};         //!< Treble beat threshold

    // Audio Processing
    float volumeBoost{1.0f};             //!< Volume boost multiplier
    bool normalize{false};               //!< Normalize audio levels
    bool enableSmoothing{true};          //!< Smooth audio values

    // Feature Extraction
    bool enableBPMDetection{true};       //!< Detect BPM
    bool enableKeyDetection{false};      //!< Detect musical key
    bool enableOnsetDetection{true};     //!< Detect transients/onsets

    // Audio Reactivity
    float energyResponseSpeed{1.0f};     //!< How fast energy responds (0.1 - 3.0)
    float beatDecayRate{0.95f};          //!< How fast beat signal decays

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = AudioSettings{};
    }

    /**
     * @brief Validate and clamp values
     */
    void Validate()
    {
        // Clamp sensitivities
        if (beatSensitivity < 0.1f) beatSensitivity = 0.1f;
        if (beatSensitivity > 5.0f) beatSensitivity = 5.0f;

        if (bassThreshold < 1.0f) bassThreshold = 1.0f;
        if (bassThreshold > 3.0f) bassThreshold = 3.0f;

        if (trebleThreshold < 1.0f) trebleThreshold = 1.0f;
        if (trebleThreshold > 3.0f) trebleThreshold = 3.0f;

        // Clamp boost
        if (volumeBoost < 0.1f) volumeBoost = 0.1f;
        if (volumeBoost > 5.0f) volumeBoost = 5.0f;

        // Clamp response speed
        if (energyResponseSpeed < 0.1f) energyResponseSpeed = 0.1f;
        if (energyResponseSpeed > 3.0f) energyResponseSpeed = 3.0f;

        // Clamp decay rate
        if (beatDecayRate < 0.5f) beatDecayRate = 0.5f;
        if (beatDecayRate > 0.99f) beatDecayRate = 0.99f;
    }
};

} // namespace Settings
} // namespace libprojectM
