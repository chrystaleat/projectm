/**
 * @file SettingsUI.cpp
 * @brief Implementation of ImGui-based settings UI
 */

#include "SettingsUI.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <projectM-4/parameters.h>

#include <SDL2/SDL.h>

namespace libprojectM {
namespace Settings {

// Preset definitions
static const char* BoidsPresetNames[] = {
    "Custom",
    "Scattered (High Separation)",
    "Tight Flock (High Cohesion)",
    "Clustered (Balanced)",
    "Natural (Moderate)"
};

static const char* RDPresetNames[] = {
    "Custom",
    "Spots",
    "Stripes",
    "Spirals",
    "Chaos",
    "Waves"
};

static const char* Preset4DNames[] = {
    "Custom",
    "Wireframe",
    "Vertices Only",
    "Hybrid",
    "Solid Faces"
};

static const char* JuliaPresetNames[] = {
    "Custom",
    "Classic",
    "Spiral",
    "Dragon",
    "Bouquet",
    "Dendrite",
    "Explosion"
};

SettingsUI::SettingsUI(projectm_handle projectMHandle, SettingsManager& settingsManager)
    : m_projectMHandle(projectMHandle)
    , m_settingsManager(settingsManager)
    , m_initialized(false)
    , m_visible(false)
    , m_window(nullptr)
    , m_glContext(nullptr)
    , m_selectedTab(0)
    , m_showSaveDialog(false)
    , m_showLoadDialog(false)
    , m_boidsPresetSelection(0)
    , m_rdPresetSelection(0)
    , m_4dPresetSelection(0)
    , m_juliaPresetSelection(0)
{
    m_saveFilename[0] = '\0';
    m_loadFilename[0] = '\0';

    // Initialize core settings from projectM
    memset(&m_coreSettings, 0, sizeof(m_coreSettings));
}

SettingsUI::~SettingsUI()
{
    if (m_initialized)
    {
        Shutdown();
    }
}

bool SettingsUI::Init(SDL_Window* window, SDL_GLContext glContext)
{
    if (m_initialized)
    {
        return true;
    }

    m_window = window;
    m_glContext = glContext;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
#if defined(__APPLE__)
    const char* glsl_version = "#version 150";
#elif defined(_WIN32)
    const char* glsl_version = "#version 130";
#else
    const char* glsl_version = "#version 130";
#endif

    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load core settings from projectM
    if (m_projectMHandle)
    {
        m_coreSettings.meshX = static_cast<int>(projectm_get_mesh_x(m_projectMHandle));
        m_coreSettings.meshY = static_cast<int>(projectm_get_mesh_y(m_projectMHandle));
        m_coreSettings.windowWidth = static_cast<int>(projectm_get_window_width(m_projectMHandle));
        m_coreSettings.windowHeight = static_cast<int>(projectm_get_window_height(m_projectMHandle));
        m_coreSettings.textureSize = 1024; // Default
        m_coreSettings.fps = static_cast<int>(projectm_get_fps(m_projectMHandle));
        m_coreSettings.presetDuration = projectm_get_preset_duration(m_projectMHandle);
        m_coreSettings.aspectCorrection = projectm_get_aspect_correction(m_projectMHandle);
        m_coreSettings.beatSensitivity = projectm_get_beat_sensitivity(m_projectMHandle);
        m_coreSettings.hardCutsEnabled = false; // Not in API
        m_coreSettings.hardCutDuration = 60.0f;
        m_coreSettings.hardCutSensitivity = 1.0f;
        m_coreSettings.smoothTransitionDuration = 3.0f; // Default
        m_coreSettings.easterEgg = 1.0f;
        m_coreSettings.fullscreen = false;
    }

    m_initialized = true;
    return true;
}

void SettingsUI::Shutdown()
{
    if (!m_initialized)
    {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    m_initialized = false;
}

bool SettingsUI::ProcessEvent(SDL_Event* event)
{
    if (!m_initialized || !m_visible)
    {
        return false;
    }

    ImGui_ImplSDL2_ProcessEvent(event);

    ImGuiIO& io = ImGui::GetIO();

    // Consume keyboard events if ImGui wants input
    if (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP)
    {
        if (io.WantCaptureKeyboard)
        {
            return true;
        }

        // Handle F1 toggle
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_F1)
        {
            ToggleVisibility();
            return true;
        }
    }

    // Consume mouse events if ImGui wants input
    if (event->type >= SDL_MOUSEMOTION && event->type <= SDL_MOUSEWHEEL)
    {
        if (io.WantCaptureMouse)
        {
            return true;
        }
    }

    return false;
}

void SettingsUI::NewFrame()
{
    if (!m_initialized || !m_visible)
    {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void SettingsUI::Render()
{
    if (!m_initialized || !m_visible)
    {
        return;
    }

    // Main settings window
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("projectM Settings", &m_visible, ImGuiWindowFlags_MenuBar))
    {
        RenderMenuBar();

        // Tab bar for different settings categories
        if (ImGui::BeginTabBar("SettingsTabs"))
        {
            if (ImGui::BeginTabItem("General"))
            {
                RenderGeneralSettings();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Audio"))
            {
                RenderAudioSettings();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Boids Flocking"))
            {
                RenderBoidsSettings();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Reaction-Diffusion"))
            {
                RenderReactionDiffusionSettings();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("4D Visualization"))
            {
                RenderHyperdimensional4DSettings();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Julia Sets"))
            {
                RenderJuliaSetSettings();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();

    // File dialogs
    if (m_showSaveDialog)
    {
        ShowSaveDialog();
    }

    if (m_showLoadDialog)
    {
        ShowLoadDialog();
    }
}

void SettingsUI::EndFrame()
{
    if (!m_initialized || !m_visible)
    {
        return;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SettingsUI::RenderMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Settings..."))
            {
                m_showSaveDialog = true;
            }

            if (ImGui::MenuItem("Load Settings..."))
            {
                m_showLoadDialog = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Reset All to Defaults"))
            {
                m_settingsManager.ResetAll();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Close", "F1"))
            {
                m_visible = false;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::Text("projectM Settings UI");
            ImGui::Separator();
            ImGui::Text("Press F1 to toggle this window");
            ImGui::Text("ESC to close");

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void SettingsUI::RenderGeneralSettings()
{
    ImGui::TextWrapped("General visualization and window settings");
    ImGui::Separator();

    ImGui::Text("Window Settings");
    if (RenderIntSlider("Window Width", &m_coreSettings.windowWidth, 320, 3840, "Window width in pixels"))
    {
        // Note: Window resize would require SDL window resize
    }

    if (RenderIntSlider("Window Height", &m_coreSettings.windowHeight, 240, 2160, "Window height in pixels"))
    {
        // Note: Window resize would require SDL window resize
    }

    RenderBoolSetting("Fullscreen", &m_coreSettings.fullscreen, "Enable fullscreen mode");
    RenderBoolSetting("Aspect Correction", &m_coreSettings.aspectCorrection, "Correct aspect ratio for non-square windows");

    if (m_coreSettings.aspectCorrection != projectm_get_aspect_correction(m_projectMHandle))
    {
        projectm_set_aspect_correction(m_projectMHandle, m_coreSettings.aspectCorrection);
    }

    ImGui::Separator();
    ImGui::Text("Performance");

    if (RenderIntSlider("Target FPS", &m_coreSettings.fps, 30, 144, "Target frames per second"))
    {
        projectm_set_fps(m_projectMHandle, static_cast<uint32_t>(m_coreSettings.fps));
    }

    if (RenderIntSlider("Mesh X", &m_coreSettings.meshX, 8, 300, "Horizontal mesh resolution"))
    {
        projectm_set_mesh_size(m_projectMHandle, static_cast<size_t>(m_coreSettings.meshX), static_cast<size_t>(m_coreSettings.meshY));
    }

    if (RenderIntSlider("Mesh Y", &m_coreSettings.meshY, 8, 300, "Vertical mesh resolution"))
    {
        projectm_set_mesh_size(m_projectMHandle, static_cast<size_t>(m_coreSettings.meshX), static_cast<size_t>(m_coreSettings.meshY));
    }

    RenderIntSlider("Texture Size", &m_coreSettings.textureSize, 256, 2048, "Internal texture resolution");

    ImGui::Separator();
    ImGui::Text("Preset Behavior");

    if (RenderFloatSlider("Preset Duration", &m_coreSettings.presetDuration, 5.0f, 300.0f, "Seconds to display each preset"))
    {
        projectm_set_preset_duration(m_projectMHandle, m_coreSettings.presetDuration);
    }

    RenderFloatSlider("Smooth Transition Duration", &m_coreSettings.smoothTransitionDuration, 0.0f, 10.0f,
                     "Blending time for smooth preset transitions");

    RenderBoolSetting("Hard Cuts Enabled", &m_coreSettings.hardCutsEnabled, "Enable beat-triggered instant preset switches");

    if (m_coreSettings.hardCutsEnabled)
    {
        RenderFloatSlider("Hard Cut Duration", &m_coreSettings.hardCutDuration, 10.0f, 300.0f,
                         "Minimum seconds before hard cut can trigger");
        RenderFloatSlider("Hard Cut Sensitivity", &m_coreSettings.hardCutSensitivity, 0.1f, 5.0f,
                         "Volume threshold for triggering hard cuts");
    }

    RenderFloatSlider("Easter Egg Parameter", &m_coreSettings.easterEgg, 0.0f, 2.0f,
                     "Random preset duration modifier");
}

void SettingsUI::RenderAudioSettings()
{
    ImGui::TextWrapped("Audio processing and beat detection settings");
    ImGui::Separator();

    ImGui::Text("Beat Detection");

    if (RenderFloatSlider("Beat Sensitivity", &m_coreSettings.beatSensitivity, 0.0f, 5.0f,
                         "Sensitivity to beats in music"))
    {
        projectm_set_beat_sensitivity(m_projectMHandle, m_coreSettings.beatSensitivity);
    }

    ImGui::Separator();
    ImGui::Text("Audio Reactivity");

    ImGui::TextWrapped("These settings control how visualizations respond to audio input.");
    ImGui::TextWrapped("Higher values create more dramatic reactions to music.");
}

void SettingsUI::RenderBoidsSettings()
{
    auto& boids = m_settingsManager.GetBoidsSettings();

    ImGui::TextWrapped("Flocking behavior simulation settings");
    ImGui::Separator();

    // Preset selector
    ImGui::Text("Presets");
    if (ImGui::Combo("Boids Preset", &m_boidsPresetSelection, BoidsPresetNames, IM_ARRAYSIZE(BoidsPresetNames)))
    {
        if (m_boidsPresetSelection > 0) // Not "Custom"
        {
            ApplyBoidsPreset(m_boidsPresetSelection);
            m_settingsManager.NotifyChange(SettingsCategory::Boids);
        }
    }

    ImGui::Separator();
    ImGui::Text("Population");

    RenderIntSlider("Max Boids", &boids.maxBoids, 10, 500, "Maximum number of boids in the simulation");
    RenderIntSlider("Spawn Count", &boids.spawnCount, 10, 500, "Number of boids to spawn initially");

    ImGui::Separator();
    ImGui::Text("Behavior Weights");

    if (RenderFloatSlider("Separation Weight", &boids.separationWeight, 0.0f, 3.0f, "Avoid crowding neighbors"))
    {
        m_boidsPresetSelection = 0; // Mark as custom
    }

    if (RenderFloatSlider("Alignment Weight", &boids.alignmentWeight, 0.0f, 3.0f, "Steer towards average heading"))
    {
        m_boidsPresetSelection = 0;
    }

    if (RenderFloatSlider("Cohesion Weight", &boids.cohesionWeight, 0.0f, 3.0f, "Steer towards center of mass"))
    {
        m_boidsPresetSelection = 0;
    }

    ImGui::Separator();
    ImGui::Text("Movement");

    RenderFloatSlider("Max Speed", &boids.maxSpeed, 0.5f, 10.0f, "Maximum speed of each boid");
    RenderFloatSlider("Max Force", &boids.maxForce, 0.01f, 0.5f, "Maximum steering force");

    ImGui::Separator();
    ImGui::Text("Perception");

    RenderFloatSlider("Separation Radius", &boids.separationRadius, 10.0f, 100.0f, "Distance to maintain from neighbors");
    RenderFloatSlider("Alignment Radius", &boids.alignmentRadius, 20.0f, 150.0f, "Distance to match neighbor heading");
    RenderFloatSlider("Cohesion Radius", &boids.cohesionRadius, 20.0f, 150.0f, "Distance to move towards neighbors");

    ImGui::Separator();
    ImGui::Text("Audio Reactivity");

    RenderBoolSetting("Audio Reactive Separation", &boids.audioReactiveSeparation, "Separation affected by beat");
    RenderBoolSetting("Audio Reactive Speed", &boids.audioReactiveSpeed, "Speed affected by volume");
    RenderBoolSetting("Audio Reactive Spawn", &boids.audioReactiveSpawn, "Spawn new boids on beat");

    if (ImGui::Button("Reset to Defaults"))
    {
        m_settingsManager.ResetToDefaults(SettingsCategory::Boids);
        m_boidsPresetSelection = 0;
    }
}

void SettingsUI::RenderReactionDiffusionSettings()
{
    auto& rd = m_settingsManager.GetReactionDiffusionSettings();

    ImGui::TextWrapped("Gray-Scott reaction-diffusion pattern generation");
    ImGui::Separator();

    // Preset selector
    ImGui::Text("Presets");
    if (ImGui::Combo("RD Preset", &m_rdPresetSelection, RDPresetNames, IM_ARRAYSIZE(RDPresetNames)))
    {
        if (m_rdPresetSelection > 0)
        {
            ApplyReactionDiffusionPreset(m_rdPresetSelection);
            m_settingsManager.NotifyChange(SettingsCategory::ReactionDiffusion);
        }
    }

    ImGui::Separator();
    ImGui::Text("Grid Resolution");

    RenderIntSlider("Grid Width", &rd.gridWidth, 64, 512, "Horizontal resolution of simulation grid");
    RenderIntSlider("Grid Height", &rd.gridHeight, 64, 512, "Vertical resolution of simulation grid");

    ImGui::Separator();
    ImGui::Text("Gray-Scott Parameters");

    if (RenderFloatSlider("Feed Rate (F)", &rd.feedRate, 0.00f, 0.12f, "Rate of chemical A addition"))
    {
        m_rdPresetSelection = 0;
    }

    if (RenderFloatSlider("Kill Rate (k)", &rd.killRate, 0.03f, 0.08f, "Rate of chemical B removal"))
    {
        m_rdPresetSelection = 0;
    }

    RenderFloatSlider("Diffusion U (Du)", &rd.diffusionU, 0.01f, 2.0f, "Diffusion rate of chemical U");
    RenderFloatSlider("Diffusion V (Dv)", &rd.diffusionV, 0.01f, 1.0f, "Diffusion rate of chemical V");

    ImGui::Separator();
    ImGui::Text("Simulation");

    RenderFloatSlider("Timestep", &rd.timestep, 0.1f, 2.0f, "Simulation speed multiplier");
    RenderIntSlider("Iterations/Frame", &rd.iterationsPerFrame, 1, 50, "Simulation steps per render frame");
    RenderBoolSetting("Auto Seed", &rd.autoSeed, "Automatically seed new patterns");

    ImGui::Separator();
    ImGui::Text("Audio Reactivity");

    RenderBoolSetting("Audio Reactive Feed Rate", &rd.audioReactiveFeedRate, "Feed rate affected by beat");
    RenderBoolSetting("Audio Reactive Kill Rate", &rd.audioReactiveKillRate, "Kill rate affected by volume");
    RenderBoolSetting("Audio Reactive Diffusion", &rd.audioReactiveDiffusion, "Diffusion affected by treble");

    if (ImGui::Button("Reset to Defaults"))
    {
        m_settingsManager.ResetToDefaults(SettingsCategory::ReactionDiffusion);
        m_rdPresetSelection = 0;
    }
}

void SettingsUI::RenderHyperdimensional4DSettings()
{
    auto& hd4d = m_settingsManager.GetHyperdimensional4DSettings();

    ImGui::TextWrapped("4D tesseract (hypercube) visualization settings");
    ImGui::Separator();

    // Preset selector
    ImGui::Text("Presets");
    if (ImGui::Combo("4D Preset", &m_4dPresetSelection, Preset4DNames, IM_ARRAYSIZE(Preset4DNames)))
    {
        if (m_4dPresetSelection > 0)
        {
            Apply4DPreset(m_4dPresetSelection);
            m_settingsManager.NotifyChange(SettingsCategory::Hyperdimensional4D);
        }
    }

    ImGui::Separator();
    ImGui::Text("Tesseract Properties");

    RenderFloatSlider("Tesseract Size", &hd4d.tesseractSize, 0.5f, 5.0f, "Size of the 4D hypercube");
    RenderFloatSlider("Edge Thickness", &hd4d.edgeThickness, 0.5f, 5.0f, "Thickness of edges");
    RenderFloatSlider("Vertex Size", &hd4d.vertexSize, 1.0f, 15.0f, "Size of vertices");

    const char* renderModes[] = { "Wireframe", "Vertices Only", "Hybrid", "Solid Faces" };
    if (RenderCombo("Render Mode", &hd4d.renderMode, renderModes, 4, "Visual style for rendering"))
    {
        m_4dPresetSelection = 0;
    }

    ImGui::Separator();
    ImGui::Text("4D Camera");

    RenderFloatSlider("4D Distance", &hd4d.camera4DDistance, 2.0f, 15.0f, "Distance of 4D camera from tesseract");
    RenderFloatSlider("4D FOV", &hd4d.camera4DFOV, 20.0f, 120.0f, "Field of view in 4D space");

    ImGui::Separator();
    ImGui::Text("4D Rotation (radians/sec)");

    RenderFloatSlider("XY Plane Rotation", &hd4d.rotationXY, -2.0f, 2.0f, "Rotation in XY plane");
    RenderFloatSlider("ZW Plane Rotation", &hd4d.rotationZW, -2.0f, 2.0f, "Rotation in ZW plane (4th dimension!)");
    RenderFloatSlider("XZ Plane Rotation", &hd4d.rotationXZ, -2.0f, 2.0f, "Rotation in XZ plane");
    RenderFloatSlider("YW Plane Rotation", &hd4d.rotationYW, -2.0f, 2.0f, "Rotation in YW plane (4th dimension!)");

    ImGui::Separator();
    ImGui::Text("Rendering");

    RenderFloatSlider("Color Cycle Speed", &hd4d.colorCycleSpeed, 0.0f, 2.0f, "Speed of color animation");
    RenderBoolSetting("Perspective Projection", &hd4d.perspectiveProjection, "Use perspective vs. orthographic projection");

    ImGui::Separator();
    ImGui::Text("Audio Reactivity");

    RenderBoolSetting("Audio Reactive Rotation", &hd4d.audioReactiveRotation, "Rotation speeds affected by beat");

    if (ImGui::Button("Reset to Defaults"))
    {
        m_settingsManager.ResetToDefaults(SettingsCategory::Hyperdimensional4D);
        m_4dPresetSelection = 0;
    }
}

void SettingsUI::RenderJuliaSetSettings()
{
    auto& julia = m_settingsManager.GetJuliaSetSettings();

    ImGui::TextWrapped("4D Julia set fractal visualization (quaternion iteration)");
    ImGui::Separator();

    // Preset selector
    ImGui::Text("Presets");
    if (ImGui::Combo("Julia Preset", &m_juliaPresetSelection, JuliaPresetNames, IM_ARRAYSIZE(JuliaPresetNames)))
    {
        if (m_juliaPresetSelection > 0)
        {
            ApplyJuliaSetPreset(m_juliaPresetSelection);
            m_settingsManager.NotifyChange(SettingsCategory::JuliaSet);
        }
    }

    ImGui::Separator();
    ImGui::Text("Quaternion Parameter c = (a + bi + cj + dk)");

    if (RenderFloatSlider("c.a (real)", &julia.c_a, -1.0f, 1.0f, "Real component of quaternion c"))
    {
        m_juliaPresetSelection = 0;
    }

    if (RenderFloatSlider("c.b (i)", &julia.c_b, -1.0f, 1.0f, "i component of quaternion c"))
    {
        m_juliaPresetSelection = 0;
    }

    if (RenderFloatSlider("c.c (j)", &julia.c_c, -1.0f, 1.0f, "j component of quaternion c"))
    {
        m_juliaPresetSelection = 0;
    }

    if (RenderFloatSlider("c.d (k)", &julia.c_d, -1.0f, 1.0f, "k component of quaternion c"))
    {
        m_juliaPresetSelection = 0;
    }

    ImGui::Separator();
    ImGui::Text("Quality");

    RenderIntSlider("Max Iterations", &julia.maxIterations, 10, 200, "Maximum iterations for escape test");
    RenderIntSlider("Sample Count", &julia.sampleCount, 10000, 500000, "Number of points to render");

    ImGui::Separator();
    ImGui::Text("Rendering");

    RenderFloatSlider("Escape Radius", &julia.escapeRadius, 1.0f, 10.0f, "Bailout radius for iteration");
    RenderFloatSlider("Point Size", &julia.pointSize, 0.5f, 5.0f, "Size of rendered points");

    ImGui::Separator();
    ImGui::Text("Bounds");

    RenderFloatSlider("Min Bound", &julia.minBound, -5.0f, 0.0f, "Minimum coordinate value");
    RenderFloatSlider("Max Bound", &julia.maxBound, 0.0f, 5.0f, "Maximum coordinate value");

    ImGui::Separator();
    ImGui::Text("Animation");

    RenderFloatSlider("c Parameter Speed", &julia.cParameterSpeed, 0.0f, 1.0f, "Speed of c parameter animation");
    RenderBoolSetting("Auto Rotate", &julia.autoRotate, "Automatically rotate the fractal");

    ImGui::Separator();
    ImGui::Text("Audio Reactivity");

    RenderBoolSetting("Audio Reactive c Parameter", &julia.audioReactiveCParameter, "c parameter affected by beat");
    RenderBoolSetting("Audio Reactive Iterations", &julia.audioReactiveIterations, "Iteration count affected by volume");

    if (ImGui::Button("Reset to Defaults"))
    {
        m_settingsManager.ResetToDefaults(SettingsCategory::JuliaSet);
        m_juliaPresetSelection = 0;
    }
}

// Helper methods
void SettingsUI::RenderBoolSetting(const char* label, bool* value, const char* tooltip)
{
    ImGui::Checkbox(label, value);
    if (tooltip && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tooltip);
    }
}

bool SettingsUI::RenderIntSlider(const char* label, int* value, int min, int max, const char* tooltip)
{
    bool changed = ImGui::SliderInt(label, value, min, max);
    if (tooltip && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tooltip);
    }
    return changed;
}

bool SettingsUI::RenderFloatSlider(const char* label, float* value, float min, float max, const char* tooltip)
{
    bool changed = ImGui::SliderFloat(label, value, min, max, "%.3f");
    if (tooltip && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tooltip);
    }
    return changed;
}

bool SettingsUI::RenderCombo(const char* label, int* currentItem, const char* const* items, int itemCount, const char* tooltip)
{
    bool changed = ImGui::Combo(label, currentItem, items, itemCount);
    if (tooltip && ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", tooltip);
    }
    return changed;
}

// Preset application
void SettingsUI::ApplyBoidsPreset(int presetIndex)
{
    auto& boids = m_settingsManager.GetBoidsSettings();

    switch (presetIndex)
    {
        case 1: // Scattered
            boids.separationWeight = 3.0f;
            boids.alignmentWeight = 0.5f;
            boids.cohesionWeight = 0.3f;
            boids.separationRadius = 50.0f;
            break;

        case 2: // Tight Flock
            boids.separationWeight = 0.5f;
            boids.alignmentWeight = 2.0f;
            boids.cohesionWeight = 2.5f;
            boids.cohesionRadius = 100.0f;
            break;

        case 3: // Clustered
            boids.separationWeight = 1.2f;
            boids.alignmentWeight = 1.2f;
            boids.cohesionWeight = 1.2f;
            break;

        case 4: // Natural
            boids.separationWeight = 1.5f;
            boids.alignmentWeight = 1.0f;
            boids.cohesionWeight = 1.0f;
            break;
    }
}

void SettingsUI::ApplyReactionDiffusionPreset(int presetIndex)
{
    auto& rd = m_settingsManager.GetReactionDiffusionSettings();

    switch (presetIndex)
    {
        case 1: // Spots
            rd.feedRate = 0.055f;
            rd.killRate = 0.062f;
            break;

        case 2: // Stripes
            rd.feedRate = 0.035f;
            rd.killRate = 0.060f;
            break;

        case 3: // Spirals
            rd.feedRate = 0.029f;
            rd.killRate = 0.057f;
            break;

        case 4: // Chaos
            rd.feedRate = 0.026f;
            rd.killRate = 0.051f;
            break;

        case 5: // Waves
            rd.feedRate = 0.014f;
            rd.killRate = 0.054f;
            break;
    }
}

void SettingsUI::Apply4DPreset(int presetIndex)
{
    auto& hd4d = m_settingsManager.GetHyperdimensional4DSettings();

    switch (presetIndex)
    {
        case 1: // Wireframe
            hd4d.renderMode = 0;
            hd4d.edgeThickness = 2.0f;
            break;

        case 2: // Vertices Only
            hd4d.renderMode = 1;
            hd4d.vertexSize = 8.0f;
            break;

        case 3: // Hybrid
            hd4d.renderMode = 2;
            hd4d.edgeThickness = 1.5f;
            hd4d.vertexSize = 5.0f;
            break;

        case 4: // Solid Faces
            hd4d.renderMode = 3;
            break;
    }
}

void SettingsUI::ApplyJuliaSetPreset(int presetIndex)
{
    auto& julia = m_settingsManager.GetJuliaSetSettings();

    switch (presetIndex)
    {
        case 1: // Classic
            julia.c_a = -0.2f;
            julia.c_b = 0.65f;
            julia.c_c = -0.3f;
            julia.c_d = 0.4f;
            break;

        case 2: // Spiral
            julia.c_a = 0.3f;
            julia.c_b = 0.5f;
            julia.c_c = -0.4f;
            julia.c_d = 0.2f;
            break;

        case 3: // Dragon
            julia.c_a = -0.8f;
            julia.c_b = 0.156f;
            julia.c_c = 0.0f;
            julia.c_d = 0.0f;
            break;

        case 4: // Bouquet
            julia.c_a = 0.28f;
            julia.c_b = -0.008f;
            julia.c_c = 0.35f;
            julia.c_d = -0.3f;
            break;

        case 5: // Dendrite
            julia.c_a = 0.0f;
            julia.c_b = 0.0f;
            julia.c_c = 0.8f;
            julia.c_d = 0.0f;
            break;

        case 6: // Explosion
            julia.c_a = -0.4f;
            julia.c_b = -0.59f;
            julia.c_c = 0.0f;
            julia.c_d = 0.0f;
            break;
    }
}

void SettingsUI::ShowSaveDialog()
{
    ImGui::OpenPopup("Save Settings");

    if (ImGui::BeginPopupModal("Save Settings", &m_showSaveDialog))
    {
        ImGui::Text("Enter filename:");
        ImGui::InputText("##filename", m_saveFilename, sizeof(m_saveFilename));

        if (ImGui::Button("Save"))
        {
            std::string filepath = std::string(m_saveFilename);
            if (!filepath.empty())
            {
                if (filepath.find(".ini") == std::string::npos)
                {
                    filepath += ".ini";
                }

                if (m_settingsManager.SaveToFile(filepath))
                {
                    ImGui::CloseCurrentPopup();
                    m_showSaveDialog = false;
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            m_showSaveDialog = false;
        }

        ImGui::EndPopup();
    }
}

void SettingsUI::ShowLoadDialog()
{
    ImGui::OpenPopup("Load Settings");

    if (ImGui::BeginPopupModal("Load Settings", &m_showLoadDialog))
    {
        ImGui::Text("Enter filename:");
        ImGui::InputText("##filename", m_loadFilename, sizeof(m_loadFilename));

        if (ImGui::Button("Load"))
        {
            std::string filepath = std::string(m_loadFilename);
            if (!filepath.empty())
            {
                if (m_settingsManager.LoadFromFile(filepath))
                {
                    ImGui::CloseCurrentPopup();
                    m_showLoadDialog = false;

                    // Notify all categories changed
                    m_settingsManager.NotifyChange(SettingsCategory::Boids);
                    m_settingsManager.NotifyChange(SettingsCategory::ReactionDiffusion);
                    m_settingsManager.NotifyChange(SettingsCategory::Hyperdimensional4D);
                    m_settingsManager.NotifyChange(SettingsCategory::JuliaSet);
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
            m_showLoadDialog = false;
        }

        ImGui::EndPopup();
    }
}

} // namespace Settings
} // namespace libprojectM
