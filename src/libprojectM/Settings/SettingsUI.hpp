/**
 * @file SettingsUI.hpp
 * @brief ImGui-based settings user interface
 */

#pragma once

#include "SettingsManager.hpp"

#include <projectM-4/projectM_export.h>

#include <functional>
#include <string>

// Forward declarations for ImGui
struct ImGuiContext;
struct SDL_Window;
typedef void* SDL_GLContext;
union SDL_Event;

namespace libprojectM {
namespace Settings {

/**
 * @brief ImGui-based settings UI
 *
 * Provides a beautiful, comprehensive settings interface with:
 * - Tabbed layout for all 6 settings categories
 * - Rich widgets (sliders, checkboxes, combo boxes)
 * - Preset selectors with one-click application
 * - Save/Load/Reset functionality
 * - Live preview integration
 * - Tooltips and help text
 * - Hotkey support (F1 to toggle)
 *
 * Usage:
 * @code
 * SettingsUI ui(settingsManager);
 * ui.Init(window, glContext);
 *
 * // In render loop:
 * ui.ProcessEvent(event);
 * ui.Render();
 *
 * ui.Shutdown();
 * @endcode
 */
class PROJECTM_EXPORT SettingsUI
{
public:
    /**
     * @brief Construct settings UI
     *
     * @param settingsManager Reference to settings manager
     */
    explicit SettingsUI(SettingsManager& settingsManager);

    /**
     * @brief Destructor
     */
    ~SettingsUI();

    // ====== Lifecycle ======

    /**
     * @brief Initialize ImGui and backends
     *
     * @param window SDL window
     * @param glContext OpenGL context
     * @return true if successful
     */
    bool Init(SDL_Window* window, SDL_GLContext glContext);

    /**
     * @brief Shutdown ImGui and cleanup
     */
    void Shutdown();

    // ====== Event Handling ======

    /**
     * @brief Process SDL event
     *
     * Should be called for all SDL events before app processes them.
     * Returns true if event was consumed by UI.
     *
     * @param event SDL event
     * @return true if event was consumed by UI
     */
    bool ProcessEvent(SDL_Event* event);

    // ====== Rendering ======

    /**
     * @brief Begin new ImGui frame
     *
     * Call before rendering UI.
     */
    void NewFrame();

    /**
     * @brief Render settings UI
     *
     * Call after NewFrame() and before EndFrame().
     */
    void Render();

    /**
     * @brief End ImGui frame and render
     *
     * Call after Render().
     */
    void EndFrame();

    // ====== UI State ======

    /**
     * @brief Check if UI is visible
     */
    bool IsVisible() const { return m_showUI; }

    /**
     * @brief Set UI visibility
     */
    void SetVisible(bool visible) { m_showUI = visible; }

    /**
     * @brief Toggle UI visibility
     */
    void ToggleVisible() { m_showUI = !m_showUI; }

    /**
     * @brief Check if UI wants to capture keyboard
     */
    bool WantsKeyboard() const;

    /**
     * @brief Check if UI wants to capture mouse
     */
    bool WantsMouse() const;

    // ====== Callbacks ======

    /**
     * @brief Callback for when settings are applied
     *
     * Called when user clicks Apply or settings change.
     */
    using ApplyCallback = std::function<void()>;

    /**
     * @brief Register callback for apply action
     */
    void RegisterApplyCallback(ApplyCallback callback)
    {
        m_applyCallback = callback;
    }

private:
    // ====== Settings Panels ======

    /**
     * @brief Render general settings panel
     */
    void RenderGeneralSettings();

    /**
     * @brief Render audio settings panel
     */
    void RenderAudioSettings();

    /**
     * @brief Render boids settings panel
     */
    void RenderBoidsSettings();

    /**
     * @brief Render reaction-diffusion settings panel
     */
    void RenderReactionDiffusionSettings();

    /**
     * @brief Render 4D visualization settings panel
     */
    void RenderHyperdimensional4DSettings();

    /**
     * @brief Render Julia set settings panel
     */
    void RenderJuliaSetSettings();

    // ====== Helper Methods ======

    /**
     * @brief Render save/load/reset buttons
     */
    void RenderFileButtons();

    /**
     * @brief Apply current settings
     */
    void ApplySettings();

    /**
     * @brief Render tooltip
     */
    void Tooltip(const char* text);

    /**
     * @brief Render preset combo box
     */
    template<typename PresetEnum>
    bool PresetCombo(const char* label, PresetEnum& preset, const char* const* presetNames, int presetCount);

    // ====== Member Variables ======

    SettingsManager& m_settingsManager;  //!< Reference to settings manager

    bool m_showUI{false};                //!< UI visibility
    int m_activeTab{0};                  //!< Active settings tab

    bool m_initialized{false};           //!< Initialization state

    ApplyCallback m_applyCallback;       //!< Apply callback

    // Temporary state for file dialogs
    std::string m_saveFilePath;          //!< Save file path
    std::string m_loadFilePath;          //!< Load file path
    bool m_showSaveDialog{false};        //!< Show save dialog
    bool m_showLoadDialog{false};        //!< Show load dialog
};

} // namespace Settings
} // namespace libprojectM
