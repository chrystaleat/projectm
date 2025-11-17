/**
 * @file SettingsManager.hpp
 * @brief Central settings management system
 */

#pragma once

#include "GeneralSettings.hpp"
#include "AudioSettings.hpp"
#include "BoidsSettings.hpp"
#include "ReactionDiffusionSettings.hpp"
#include "Hyperdimensional4DSettings.hpp"
#include "JuliaSetSettings.hpp"

#include <projectM-4/projectM_export.h>

#include <functional>
#include <string>
#include <vector>

namespace libprojectM {
namespace Settings {

/**
 * @brief Settings category enum
 */
enum class SettingsCategory
{
    General,
    Audio,
    Boids,
    ReactionDiffusion,
    Hyperdimensional4D,
    JuliaSet,
    All
};

/**
 * @brief Callback type for settings changes
 */
using SettingsChangeCallback = std::function<void(SettingsCategory)>;

/**
 * @brief Central settings management class
 *
 * Manages all application settings with:
 * - Organized categories
 * - Persistence (save/load from JSON)
 * - Validation
 * - Change notifications
 * - Reset functionality
 *
 * Usage:
 * @code
 * SettingsManager settings;
 *
 * // Modify settings
 * auto& boids = settings.GetBoidsSettings();
 * boids.maxBoids = 2000;
 *
 * // Register for change notifications
 * settings.RegisterChangeCallback([](SettingsCategory cat) {
 *     std::cout << "Settings changed: " << static_cast<int>(cat) << std::endl;
 * });
 *
 * // Save to file
 * settings.SaveToFile("settings.json");
 *
 * // Load from file
 * settings.LoadFromFile("settings.json");
 * @endcode
 */
class PROJECTM_EXPORT SettingsManager
{
public:
    /**
     * @brief Construct with default settings
     */
    SettingsManager();

    /**
     * @brief Construct and load from file
     */
    explicit SettingsManager(const std::string& configPath);

    // ====== Settings Access ======

    /**
     * @brief Get general settings (read-only)
     */
    const GeneralSettings& GetGeneralSettings() const { return m_generalSettings; }

    /**
     * @brief Get general settings (mutable)
     */
    GeneralSettings& GetGeneralSettings() { return m_generalSettings; }

    /**
     * @brief Get audio settings (read-only)
     */
    const AudioSettings& GetAudioSettings() const { return m_audioSettings; }

    /**
     * @brief Get audio settings (mutable)
     */
    AudioSettings& GetAudioSettings() { return m_audioSettings; }

    /**
     * @brief Get boids settings (read-only)
     */
    const BoidsSettings& GetBoidsSettings() const { return m_boidsSettings; }

    /**
     * @brief Get boids settings (mutable)
     */
    BoidsSettings& GetBoidsSettings() { return m_boidsSettings; }

    /**
     * @brief Get reaction-diffusion settings (read-only)
     */
    const ReactionDiffusionSettings& GetReactionDiffusionSettings() const { return m_rdSettings; }

    /**
     * @brief Get reaction-diffusion settings (mutable)
     */
    ReactionDiffusionSettings& GetReactionDiffusionSettings() { return m_rdSettings; }

    /**
     * @brief Get 4D hyperdimensional settings (read-only)
     */
    const Hyperdimensional4DSettings& GetHyperdimensional4DSettings() const { return m_hyper4DSettings; }

    /**
     * @brief Get 4D hyperdimensional settings (mutable)
     */
    Hyperdimensional4DSettings& GetHyperdimensional4DSettings() { return m_hyper4DSettings; }

    /**
     * @brief Get Julia set settings (read-only)
     */
    const JuliaSetSettings& GetJuliaSetSettings() const { return m_juliaSettings; }

    /**
     * @brief Get Julia set settings (mutable)
     */
    JuliaSetSettings& GetJuliaSetSettings() { return m_juliaSettings; }

    // ====== Validation ======

    /**
     * @brief Validate all settings
     *
     * Clamps values to acceptable ranges.
     */
    void ValidateSettings();

    /**
     * @brief Validate specific category
     */
    void ValidateCategory(SettingsCategory category);

    // ====== Persistence ======

    /**
     * @brief Save all settings to JSON file
     *
     * @param filePath Path to save file
     * @return true if successful
     */
    bool SaveToFile(const std::string& filePath) const;

    /**
     * @brief Load settings from JSON file
     *
     * @param filePath Path to load file
     * @return true if successful
     */
    bool LoadFromFile(const std::string& filePath);

    // ====== Reset ======

    /**
     * @brief Reset all settings to defaults
     */
    void ResetToDefaults();

    /**
     * @brief Reset specific category to defaults
     */
    void ResetCategory(SettingsCategory category);

    // ====== Change Notifications ======

    /**
     * @brief Register callback for settings changes
     *
     * @param callback Function to call when settings change
     */
    void RegisterChangeCallback(SettingsChangeCallback callback);

    /**
     * @brief Notify that settings changed
     *
     * Calls all registered callbacks.
     *
     * @param category Category that changed
     */
    void NotifyChange(SettingsCategory category);

private:
    // ====== Settings Storage ======

    GeneralSettings m_generalSettings;
    AudioSettings m_audioSettings;
    BoidsSettings m_boidsSettings;
    ReactionDiffusionSettings m_rdSettings;
    Hyperdimensional4DSettings m_hyper4DSettings;
    JuliaSetSettings m_juliaSettings;

    // ====== Change Callbacks ======

    std::vector<SettingsChangeCallback> m_changeCallbacks;

    // ====== Helper Methods ======

    /**
     * @brief Convert settings to JSON string
     */
    std::string ToJSON() const;

    /**
     * @brief Parse settings from JSON string
     */
    bool FromJSON(const std::string& json);
};

} // namespace Settings
} // namespace libprojectM
