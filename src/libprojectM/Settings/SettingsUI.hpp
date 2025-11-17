/**
 * @file SettingsUI.hpp
 * @brief ImGui-based settings user interface
 *
 * Provides a comprehensive Dear ImGui interface for configuring all projectM settings
 * including core settings and advanced features (Boids, Reaction-Diffusion, 4D, Julia sets).
 */

#pragma once

#include "SettingsManager.hpp"

#include <projectM-4/projectM_export.h>
#include <projectM-4/projectM.h>

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
 * @brief ImGui-based settings UI for projectM
 *
 * This class provides a comprehensive graphical interface for all projectM settings.
 * It integrates with both the core projectM API (for basic settings) and the
 * SettingsManager (for advanced features).
 */
class PROJECTM_EXPORT SettingsUI
{
public:
    /**
     * @brief Constructor
     * @param projectMHandle Handle to the projectM instance
     * @param settingsManager Reference to the settings manager for advanced features
     */
    SettingsUI(projectm_handle projectMHandle, SettingsManager& settingsManager);

    /**
     * @brief Destructor
     */
    ~SettingsUI();

    /**
     * @brief Initialize ImGui with SDL and OpenGL
     * @param window SDL window pointer
     * @param glContext OpenGL context
     * @return true if initialization succeeded
     */
    bool Init(SDL_Window* window, SDL_GLContext glContext);

    /**
     * @brief Shutdown ImGui and clean up resources
     */
    void Shutdown();

    /**
     * @brief Process SDL events for ImGui
     * @param event SDL event pointer
     * @return true if the event was consumed by ImGui (don't process further)
     */
    bool ProcessEvent(SDL_Event* event);

    /**
     * @brief Begin a new ImGui frame
     */
    void NewFrame();

    /**
     * @brief Render the settings UI
     */
    void Render();

    /**
     * @brief End the ImGui frame and render to screen
     */
    void EndFrame();

    /**
     * @brief Toggle UI visibility
     */
    void ToggleVisibility() { m_visible = !m_visible; }

    /**
     * @brief Set UI visibility
     */
    void SetVisible(bool visible) { m_visible = visible; }

    /**
     * @brief Check if UI is visible
     */
    bool IsVisible() const { return m_visible; }

private:
    // Rendering methods for each settings panel
    void RenderMenuBar();
    void RenderGeneralSettings();
    void RenderAudioSettings();
    void RenderBoidsSettings();
    void RenderReactionDiffusionSettings();
    void RenderHyperdimensional4DSettings();
    void RenderJuliaSetSettings();

    // Helper methods
    void RenderBoolSetting(const char* label, bool* value, const char* tooltip = nullptr);
    void RenderIntSlider(const char* label, int* value, int min, int max, const char* tooltip = nullptr);
    void RenderFloatSlider(const char* label, float* value, float min, float max, const char* tooltip = nullptr);
    void RenderCombo(const char* label, int* currentItem, const char* const* items, int itemCount, const char* tooltip = nullptr);

    // Preset helpers
    void ApplyBoidsPreset(int presetIndex);
    void ApplyReactionDiffusionPreset(int presetIndex);
    void Apply4DPreset(int presetIndex);
    void ApplyJuliaSetPreset(int presetIndex);

    // File dialog helpers
    void ShowSaveDialog();
    void ShowLoadDialog();

private:
    projectm_handle m_projectMHandle;
    SettingsManager& m_settingsManager;

    bool m_initialized;
    bool m_visible;

    SDL_Window* m_window;
    SDL_GLContext m_glContext;

    // UI state
    int m_selectedTab;
    char m_saveFilename[256];
    char m_loadFilename[256];
    bool m_showSaveDialog;
    bool m_showLoadDialog;

    // Temp storage for projectM core settings
    struct CoreSettings
    {
        int meshX;
        int meshY;
        int windowWidth;
        int windowHeight;
        int textureSize;
        int fps;
        float presetDuration;
        float smoothTransitionDuration;
        bool hardCutsEnabled;
        float hardCutDuration;
        float hardCutSensitivity;
        float beatSensitivity;
        float easterEgg;
        bool aspectCorrection;
        bool fullscreen;
    } m_coreSettings;

    // Preset selection state
    int m_boidsPresetSelection;
    int m_rdPresetSelection;
    int m_4dPresetSelection;
    int m_juliaPresetSelection;
};

} // namespace Settings
} // namespace libprojectM
