/**
 * @file GeneralSettings.hpp
 * @brief General application settings
 */

#pragma once

#include <projectM-4/projectM_export.h>

#include <string>

namespace libprojectM {
namespace Settings {

/**
 * @brief General application settings
 */
struct PROJECTM_EXPORT GeneralSettings
{
    // Display Settings
    int windowWidth{1920};           //!< Window width in pixels
    int windowHeight{1080};          //!< Window height in pixels
    bool fullscreen{false};          //!< Fullscreen mode
    bool vsync{true};                //!< VSync enabled

    // Performance Settings
    int fps{60};                     //!< Target frames per second
    int meshX{708};                  //!< Width of per-pixel equation mesh
    int meshY{400};                  //!< Height of per-pixel equation mesh
    int textureSize{1024};           //!< Size of internal rendering texture

    // Preset Settings
    int presetDuration{10};          //!< Preset duration in seconds
    int smoothTransitionDuration{1}; //!< Smooth transition duration in seconds
    bool hardCutsEnabled{false};     //!< Enable hard cuts
    int hardCutDuration{60};         //!< Seconds before eligible for hard cut
    float hardCutSensitivity{1.0f};  //!< Volume sensitivity for hard cuts

    // Visual Settings
    bool aspectCorrection{true};     //!< Custom shape aspect correction
    bool showFPS{false};             //!< Show FPS counter
    bool showPresetName{true};       //!< Show preset name on change

    // Paths
    std::string presetPath{"presets"};     //!< Path to preset directory
    std::string titleFont{"Vera.ttf"};     //!< Title font file
    std::string menuFont{"VeraMono.ttf"};  //!< Menu font file

    /**
     * @brief Reset to default values
     */
    void ResetToDefaults()
    {
        *this = GeneralSettings{};
    }

    /**
     * @brief Validate and clamp values to acceptable ranges
     */
    void Validate()
    {
        // Clamp FPS
        if (fps < 15) fps = 15;
        if (fps > 240) fps = 240;

        // Clamp window size
        if (windowWidth < 320) windowWidth = 320;
        if (windowHeight < 240) windowHeight = 240;
        if (windowWidth > 7680) windowWidth = 7680;  // 8K
        if (windowHeight > 4320) windowHeight = 4320;

        // Clamp mesh size
        if (meshX < 8) meshX = 8;
        if (meshY < 8) meshY = 8;
        if (meshX > 2048) meshX = 2048;
        if (meshY > 2048) meshY = 2048;

        // Clamp texture size
        if (textureSize < 256) textureSize = 256;
        if (textureSize > 4096) textureSize = 4096;

        // Clamp durations
        if (presetDuration < 1) presetDuration = 1;
        if (presetDuration > 600) presetDuration = 600;
        if (smoothTransitionDuration < 0) smoothTransitionDuration = 0;
        if (smoothTransitionDuration > 10) smoothTransitionDuration = 10;

        // Clamp sensitivities
        if (hardCutSensitivity < 0.1f) hardCutSensitivity = 0.1f;
        if (hardCutSensitivity < 5.0f) hardCutSensitivity = 5.0f;
    }
};

} // namespace Settings
} // namespace libprojectM
