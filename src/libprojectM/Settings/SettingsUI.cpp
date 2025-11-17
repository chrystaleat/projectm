/**
 * @file SettingsUI.cpp
 * @brief Implementation of ImGui-based settings UI
 */

#include "SettingsUI.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

#include <SDL2/SDL.h>

namespace libprojectM {
namespace Settings {

// Preset name arrays for combo boxes
static const char* BoidsPresetNames[] = {
    "Custom",
    "Scattered (High Separation)",
    "Tight Flock (High Cohesion)",
    "Clustered (Balanced)",
    "Natural (Default)"
};

static const char* RDPresetNames[] = {
    "Custom",
    "Spots (Classic Dots)",
    "Stripes (Zebra Patterns)",
    "Spirals (Organic Swirls)",
    "Chaos (Turbulent Patterns)",
    "Waves (Smooth Flowing)"
};

static const char* JuliaPresetNames[] = {
    "Custom",
    "Classic Julia",
    "Spiral Structures",
    "Dragon Shapes",
    "Flower Bouquet",
    "Chaotic Patterns",
    "Symmetrical Forms"
};

static const char* RenderModeNames[] = {
    "Wireframe",
    "Vertices Only",
    "Hybrid (Wire + Verts)",
    "Faces (Filled)"
};

SettingsUI::SettingsUI(SettingsManager& settingsManager)
    : m_settingsManager(settingsManager)
{
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
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Customize style for better visuals
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 5.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, glContext);
    ImGui_ImplOpenGL3_Init("#version 130");

    m_initialized = true;
    return true;
}

void SettingsUI::Shutdown()
{
    if (m_initialized)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        m_initialized = false;
    }
}

bool SettingsUI::ProcessEvent(SDL_Event* event)
{
    if (!m_initialized)
        return false;

    // Check for F1 key to toggle UI
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_F1)
    {
        ToggleVisible();
        return true;
    }

    // Pass event to ImGui
    ImGui_ImplSDL2_ProcessEvent(event);

    // Return true if ImGui wants to capture input
    return (WantsKeyboard() && (event->type == SDL_KEYDOWN || event->type == SDL_KEYUP || event->type == SDL_TEXTINPUT)) ||
           (WantsMouse() && (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP || event->type == SDL_MOUSEWHEEL));
}

void SettingsUI::NewFrame()
{
    if (!m_initialized)
        return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void SettingsUI::Render()
{
    if (!m_initialized || !m_showUI)
        return;

    // Main settings window
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("ProjectM Settings", &m_showUI, ImGuiWindowFlags_MenuBar))
    {
        // Menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Save Settings"))
                {
                    m_showSaveDialog = true;
                }
                if (ImGui::MenuItem("Load Settings"))
                {
                    m_showLoadDialog = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Reset All"))
                {
                    m_settingsManager.ResetToDefaults();
                    ApplySettings();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                ImGui::Text("ProjectM Settings Interface");
                ImGui::Separator();
                ImGui::Text("F1: Toggle this window");
                ImGui::Text("ESC: Close this window");
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Tab bar for categories
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

            if (ImGui::BeginTabItem("Boids"))
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

            if (ImGui::BeginTabItem("Julia Set"))
            {
                RenderJuliaSetSettings();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        // Bottom buttons
        ImGui::Separator();
        RenderFileButtons();
    }
    ImGui::End();

    // Simple file dialogs (can be enhanced with proper file browser later)
    if (m_showSaveDialog)
    {
        ImGui::OpenPopup("Save Settings");
        m_showSaveDialog = false;
    }

    if (m_showLoadDialog)
    {
        ImGui::OpenPopup("Load Settings");
        m_showLoadDialog = false;
    }

    // Save dialog
    if (ImGui::BeginPopupModal("Save Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char pathBuffer[256] = "settings.json";
        ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer));

        if (ImGui::Button("Save", ImVec2(120, 0)))
        {
            m_settingsManager.SaveToFile(pathBuffer);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // Load dialog
    if (ImGui::BeginPopupModal("Load Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static char pathBuffer[256] = "settings.json";
        ImGui::InputText("File Path", pathBuffer, sizeof(pathBuffer));

        if (ImGui::Button("Load", ImVec2(120, 0)))
        {
            if (m_settingsManager.LoadFromFile(pathBuffer))
            {
                ApplySettings();
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SettingsUI::EndFrame()
{
    if (!m_initialized)
        return;

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool SettingsUI::WantsKeyboard() const
{
    if (!m_initialized)
        return false;
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool SettingsUI::WantsMouse() const
{
    if (!m_initialized)
        return false;
    return ImGui::GetIO().WantCaptureMouse;
}

void SettingsUI::RenderGeneralSettings()
{
    auto& general = m_settingsManager.GetGeneralSettings();

    ImGui::SeparatorText("Window Settings");

    if (ImGui::SliderInt("Window Width", &general.windowWidth, 320, 7680))
        ApplySettings();
    Tooltip("Window width in pixels (320-7680)");

    if (ImGui::SliderInt("Window Height", &general.windowHeight, 240, 4320))
        ApplySettings();
    Tooltip("Window height in pixels (240-4320)");

    if (ImGui::Checkbox("Fullscreen", &general.fullscreen))
        ApplySettings();
    Tooltip("Enable fullscreen mode");

    if (ImGui::Checkbox("VSync", &general.vsync))
        ApplySettings();
    Tooltip("Enable vertical sync to prevent tearing");

    ImGui::SeparatorText("Performance");

    if (ImGui::SliderInt("FPS Target", &general.fps, 15, 240))
        ApplySettings();
    Tooltip("Target frames per second (15-240)");

    if (ImGui::SliderInt("Mesh Resolution", &general.meshResolution, 16, 256))
        ApplySettings();
    Tooltip("Mesh grid resolution (16-256)");

    if (ImGui::SliderInt("Texture Size", &general.textureSize, 256, 2048))
        ApplySettings();
    Tooltip("Texture resolution (256-2048)");

    ImGui::SeparatorText("Presets");

    if (ImGui::SliderInt("Preset Duration (s)", &general.presetDuration, 1, 300))
        ApplySettings();
    Tooltip("How long each preset displays (seconds)");

    if (ImGui::Checkbox("Smooth Transitions", &general.smoothTransitions))
        ApplySettings();
    Tooltip("Smooth fade between presets");

    if (ImGui::SliderFloat("Transition Duration", &general.transitionDuration, 0.1f, 10.0f, "%.1f"))
        ApplySettings();
    Tooltip("Duration of preset transitions (seconds)");

    if (ImGui::SliderInt("Hard Cut Frequency", &general.hardCutFrequency, 0, 100))
        ApplySettings();
    Tooltip("Percentage of hard cuts vs smooth transitions");

    ImGui::SeparatorText("Display");

    if (ImGui::Checkbox("Show FPS", &general.showFPS))
        ApplySettings();
    Tooltip("Display FPS counter");

    if (ImGui::Checkbox("Show Preset Name", &general.showPresetName))
        ApplySettings();
    Tooltip("Display current preset name");

    if (ImGui::Checkbox("Show Debug Info", &general.showDebugInfo))
        ApplySettings();
    Tooltip("Display debug information");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset General Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::General);
        ApplySettings();
    }
}

void SettingsUI::RenderAudioSettings()
{
    auto& audio = m_settingsManager.GetAudioSettings();

    ImGui::SeparatorText("Beat Detection");

    if (ImGui::Checkbox("Enable Beat Detection", &audio.enableBeatDetection))
        ApplySettings();
    Tooltip("Enable automatic beat detection");

    if (ImGui::SliderFloat("Beat Sensitivity", &audio.beatSensitivity, 0.1f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("How sensitive the beat detector is (0.1-5.0)");

    if (ImGui::SliderFloat("Beat Decay", &audio.beatDecay, 0.1f, 2.0f, "%.2f"))
        ApplySettings();
    Tooltip("How quickly beats fade (0.1-2.0)");

    ImGui::SeparatorText("Audio Processing");

    if (ImGui::SliderFloat("Volume Boost", &audio.volumeBoost, 0.1f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Audio volume multiplier (0.1-5.0)");

    if (ImGui::Checkbox("Normalize Audio", &audio.normalizeAudio))
        ApplySettings();
    Tooltip("Normalize audio levels");

    if (ImGui::SliderFloat("Bass Boost", &audio.bassBoost, 0.0f, 3.0f, "%.2f"))
        ApplySettings();
    Tooltip("Boost bass frequencies (0.0-3.0)");

    if (ImGui::SliderFloat("Treble Boost", &audio.trebleBoost, 0.0f, 3.0f, "%.2f"))
        ApplySettings();
    Tooltip("Boost treble frequencies (0.0-3.0)");

    ImGui::SeparatorText("Advanced Features");

    if (ImGui::Checkbox("Enable BPM Detection", &audio.enableBPMDetection))
        ApplySettings();
    Tooltip("Detect beats per minute");

    if (ImGui::Checkbox("Enable Key Detection", &audio.enableKeyDetection))
        ApplySettings();
    Tooltip("Detect musical key");

    if (ImGui::Checkbox("Enable Onset Detection", &audio.enableOnsetDetection))
        ApplySettings();
    Tooltip("Detect note onsets");

    ImGui::SeparatorText("Reactivity");

    if (ImGui::SliderFloat("Color Reactivity", &audio.colorReactivity, 0.0f, 2.0f, "%.2f"))
        ApplySettings();
    Tooltip("How much audio affects colors (0.0-2.0)");

    if (ImGui::SliderFloat("Motion Reactivity", &audio.motionReactivity, 0.0f, 2.0f, "%.2f"))
        ApplySettings();
    Tooltip("How much audio affects motion (0.0-2.0)");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset Audio Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::Audio);
        ApplySettings();
    }
}

void SettingsUI::RenderBoidsSettings()
{
    auto& boids = m_settingsManager.GetBoidsSettings();

    ImGui::SeparatorText("Boids Flocking System");

    if (ImGui::Checkbox("Enable Boids", &boids.enabled))
        ApplySettings();
    Tooltip("Enable Craig Reynolds' boids flocking simulation");

    // Preset selector
    int presetIndex = static_cast<int>(boids.preset);
    if (ImGui::Combo("Preset", &presetIndex, BoidsPresetNames, IM_ARRAYSIZE(BoidsPresetNames)))
    {
        boids.preset = static_cast<BoidsPreset>(presetIndex);
        boids.ApplyPreset(boids.preset);
        ApplySettings();
    }
    Tooltip("Choose a boids behavior preset");

    ImGui::SeparatorText("Population");

    if (ImGui::SliderInt("Max Boids", &boids.maxBoids, 10, 10000))
        ApplySettings();
    Tooltip("Maximum number of boids (10-10000)");

    if (ImGui::SliderInt("Spawn Count", &boids.spawnCount, 10, 1000))
        ApplySettings();
    Tooltip("Number of boids to spawn initially");

    ImGui::SeparatorText("Behavior Weights");

    if (ImGui::SliderFloat("Separation", &boids.separationWeight, 0.0f, 10.0f, "%.2f"))
    {
        boids.preset = BoidsPreset::Custom;
        ApplySettings();
    }
    Tooltip("Avoidance of crowding neighbors (0.0-10.0)");

    if (ImGui::SliderFloat("Alignment", &boids.alignmentWeight, 0.0f, 10.0f, "%.2f"))
    {
        boids.preset = BoidsPreset::Custom;
        ApplySettings();
    }
    Tooltip("Alignment with average heading (0.0-10.0)");

    if (ImGui::SliderFloat("Cohesion", &boids.cohesionWeight, 0.0f, 10.0f, "%.2f"))
    {
        boids.preset = BoidsPreset::Custom;
        ApplySettings();
    }
    Tooltip("Steering toward center of mass (0.0-10.0)");

    ImGui::SeparatorText("Movement");

    if (ImGui::SliderFloat("Max Speed", &boids.maxSpeed, 0.1f, 20.0f, "%.2f"))
        ApplySettings();
    Tooltip("Maximum boid speed (0.1-20.0)");

    if (ImGui::SliderFloat("Max Force", &boids.maxForce, 0.01f, 2.0f, "%.3f"))
        ApplySettings();
    Tooltip("Maximum steering force (0.01-2.0)");

    ImGui::SeparatorText("Perception");

    if (ImGui::SliderFloat("Separation Radius", &boids.separationRadius, 0.5f, 10.0f, "%.2f"))
        ApplySettings();
    Tooltip("Distance for separation behavior (0.5-10.0)");

    if (ImGui::SliderFloat("Neighbor Radius", &boids.neighborRadius, 1.0f, 20.0f, "%.2f"))
        ApplySettings();
    Tooltip("Distance to consider boids as neighbors (1.0-20.0)");

    ImGui::SeparatorText("Audio Reactivity");

    if (ImGui::Checkbox("Audio Reactive Speed", &boids.audioReactiveSpeed))
        ApplySettings();
    Tooltip("Vary speed with audio energy");

    if (ImGui::Checkbox("Audio Reactive Behavior", &boids.audioReactiveBehavior))
        ApplySettings();
    Tooltip("Change behavior weights with beats");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset Boids Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::Boids);
        ApplySettings();
    }
}

void SettingsUI::RenderReactionDiffusionSettings()
{
    auto& rd = m_settingsManager.GetReactionDiffusionSettings();

    ImGui::SeparatorText("Reaction-Diffusion System");

    if (ImGui::Checkbox("Enable RD Patterns", &rd.enabled))
        ApplySettings();
    Tooltip("Enable Gray-Scott reaction-diffusion patterns");

    // Preset selector
    int presetIndex = static_cast<int>(rd.preset);
    if (ImGui::Combo("Pattern Preset", &presetIndex, RDPresetNames, IM_ARRAYSIZE(RDPresetNames)))
    {
        rd.preset = static_cast<RDPreset>(presetIndex);
        rd.ApplyPreset(rd.preset);
        ApplySettings();
    }
    Tooltip("Choose a pattern preset");

    ImGui::SeparatorText("Grid Resolution");

    if (ImGui::SliderInt("Grid Width", &rd.gridWidth, 32, 512))
    {
        ApplySettings();
    }
    Tooltip("Grid width (power of 2, 32-512)");

    if (ImGui::SliderInt("Grid Height", &rd.gridHeight, 32, 512))
    {
        ApplySettings();
    }
    Tooltip("Grid height (power of 2, 32-512)");

    ImGui::SeparatorText("Gray-Scott Parameters");

    if (ImGui::SliderFloat("Feed Rate (F)", &rd.feedRate, 0.0f, 0.15f, "%.4f"))
    {
        rd.preset = RDPreset::Custom;
        ApplySettings();
    }
    Tooltip("Feed rate parameter (0.0-0.15)");

    if (ImGui::SliderFloat("Kill Rate (k)", &rd.killRate, 0.0f, 0.15f, "%.4f"))
    {
        rd.preset = RDPreset::Custom;
        ApplySettings();
    }
    Tooltip("Kill rate parameter (0.0-0.15)");

    if (ImGui::SliderFloat("Diffusion U (Du)", &rd.diffusionU, 0.0f, 0.3f, "%.4f"))
    {
        rd.preset = RDPreset::Custom;
        ApplySettings();
    }
    Tooltip("Diffusion rate of chemical U (0.0-0.3)");

    if (ImGui::SliderFloat("Diffusion V (Dv)", &rd.diffusionV, 0.0f, 0.2f, "%.4f"))
    {
        rd.preset = RDPreset::Custom;
        ApplySettings();
    }
    Tooltip("Diffusion rate of chemical V (0.0-0.2)");

    ImGui::SeparatorText("Simulation");

    if (ImGui::SliderFloat("Time Step", &rd.timeStep, 0.1f, 2.0f, "%.2f"))
        ApplySettings();
    Tooltip("Simulation time step (0.1-2.0)");

    if (ImGui::SliderInt("Steps Per Frame", &rd.stepsPerFrame, 1, 50))
        ApplySettings();
    Tooltip("Simulation steps per render frame (1-50)");

    ImGui::SeparatorText("Seeding");

    if (ImGui::SliderInt("Seed Count", &rd.seedCount, 1, 100))
        ApplySettings();
    Tooltip("Number of seed points (1-100)");

    if (ImGui::SliderFloat("Seed Radius", &rd.seedRadius, 1.0f, 50.0f, "%.1f"))
        ApplySettings();
    Tooltip("Radius of seed points (1.0-50.0)");

    if (ImGui::Checkbox("Random Seeding", &rd.randomSeeding))
        ApplySettings();
    Tooltip("Randomly place seed points each cycle");

    ImGui::SeparatorText("Audio Reactivity");

    if (ImGui::Checkbox("Audio Reactive Parameters", &rd.audioReactiveParams))
        ApplySettings();
    Tooltip("Vary F/k parameters with audio");

    if (ImGui::Checkbox("Audio Reactive Seeding", &rd.audioReactiveSeeding))
        ApplySettings();
    Tooltip("Trigger new seeds on beats");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset RD Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::ReactionDiffusion);
        ApplySettings();
    }
}

void SettingsUI::RenderHyperdimensional4DSettings()
{
    auto& hyper4d = m_settingsManager.GetHyperdimensional4DSettings();

    ImGui::SeparatorText("4D Hyperdimensional Visualization");

    if (ImGui::Checkbox("Enable 4D Visuals", &hyper4d.enabled))
        ApplySettings();
    Tooltip("Enable 4D tesseract and hypercube visualization");

    // Render mode selector
    int renderModeIndex = static_cast<int>(hyper4d.renderMode);
    if (ImGui::Combo("Render Mode", &renderModeIndex, RenderModeNames, IM_ARRAYSIZE(RenderModeNames)))
    {
        hyper4d.renderMode = static_cast<Hyper4DRenderMode>(renderModeIndex);
        ApplySettings();
    }
    Tooltip("How to render 4D objects");

    ImGui::SeparatorText("Tesseract Properties");

    if (ImGui::SliderFloat("Tesseract Size", &hyper4d.tesseractSize, 0.5f, 10.0f, "%.2f"))
        ApplySettings();
    Tooltip("Size of the tesseract (0.5-10.0)");

    if (ImGui::SliderFloat("Edge Thickness", &hyper4d.edgeThickness, 0.5f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Thickness of wireframe edges (0.5-5.0)");

    if (ImGui::SliderFloat("Vertex Size", &hyper4d.vertexSize, 1.0f, 10.0f, "%.2f"))
        ApplySettings();
    Tooltip("Size of vertex points (1.0-10.0)");

    ImGui::SeparatorText("4D Camera");

    if (ImGui::SliderFloat("4D Distance", &hyper4d.cameraDistance4D, 2.0f, 20.0f, "%.2f"))
        ApplySettings();
    Tooltip("Camera distance in 4D space (2.0-20.0)");

    if (ImGui::SliderFloat("FOV 4D", &hyper4d.fov4D, 30.0f, 120.0f, "%.1f"))
        ApplySettings();
    Tooltip("Field of view for 4D projection (30-120 degrees)");

    ImGui::SeparatorText("4D Rotation");

    if (ImGui::SliderFloat("XY Rotation Speed", &hyper4d.rotationSpeedXY, -5.0f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Rotation speed in XY plane (-5.0 to 5.0)");

    if (ImGui::SliderFloat("ZW Rotation Speed", &hyper4d.rotationSpeedZW, -5.0f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Rotation speed in ZW plane (-5.0 to 5.0)");

    if (ImGui::SliderFloat("XZ Rotation Speed", &hyper4d.rotationSpeedXZ, -5.0f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Rotation speed in XZ plane (-5.0 to 5.0)");

    if (ImGui::SliderFloat("YW Rotation Speed", &hyper4d.rotationSpeedYW, -5.0f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Rotation speed in YW plane (-5.0 to 5.0)");

    ImGui::SeparatorText("Colors");

    float edgeColor[4] = {hyper4d.edgeColor.r, hyper4d.edgeColor.g, hyper4d.edgeColor.b, hyper4d.edgeColor.a};
    if (ImGui::ColorEdit4("Edge Color", edgeColor))
    {
        hyper4d.edgeColor = {edgeColor[0], edgeColor[1], edgeColor[2], edgeColor[3]};
        ApplySettings();
    }

    float vertexColor[4] = {hyper4d.vertexColor.r, hyper4d.vertexColor.g, hyper4d.vertexColor.b, hyper4d.vertexColor.a};
    if (ImGui::ColorEdit4("Vertex Color", vertexColor))
    {
        hyper4d.vertexColor = {vertexColor[0], vertexColor[1], vertexColor[2], vertexColor[3]};
        ApplySettings();
    }

    ImGui::SeparatorText("Audio Reactivity");

    if (ImGui::Checkbox("Audio Reactive Rotation", &hyper4d.audioReactiveRotation))
        ApplySettings();
    Tooltip("Vary rotation speed with audio");

    if (ImGui::Checkbox("Audio Reactive Size", &hyper4d.audioReactiveSize))
        ApplySettings();
    Tooltip("Pulse size with beats");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset 4D Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::Hyperdimensional4D);
        ApplySettings();
    }
}

void SettingsUI::RenderJuliaSetSettings()
{
    auto& julia = m_settingsManager.GetJuliaSetSettings();

    ImGui::SeparatorText("Quaternion Julia Set Fractals");

    if (ImGui::Checkbox("Enable Julia Sets", &julia.enabled))
        ApplySettings();
    Tooltip("Enable 4D quaternion Julia set fractals");

    // Preset selector
    int presetIndex = static_cast<int>(julia.preset);
    if (ImGui::Combo("Fractal Preset", &presetIndex, JuliaPresetNames, IM_ARRAYSIZE(JuliaPresetNames)))
    {
        julia.preset = static_cast<JuliaPreset>(presetIndex);
        // Apply preset (would need to implement this in JuliaSetSettings)
        ApplySettings();
    }
    Tooltip("Choose a Julia set preset");

    ImGui::SeparatorText("Quaternion c Parameter");
    ImGui::Text("c = a + bi + cj + dk");

    if (ImGui::SliderFloat("c.a (real)", &julia.c_a, -2.0f, 2.0f, "%.3f"))
    {
        julia.preset = JuliaPreset::Custom;
        ApplySettings();
    }

    if (ImGui::SliderFloat("c.b (i)", &julia.c_b, -2.0f, 2.0f, "%.3f"))
    {
        julia.preset = JuliaPreset::Custom;
        ApplySettings();
    }

    if (ImGui::SliderFloat("c.c (j)", &julia.c_c, -2.0f, 2.0f, "%.3f"))
    {
        julia.preset = JuliaPreset::Custom;
        ApplySettings();
    }

    if (ImGui::SliderFloat("c.d (k)", &julia.c_d, -2.0f, 2.0f, "%.3f"))
    {
        julia.preset = JuliaPreset::Custom;
        ApplySettings();
    }

    ImGui::SeparatorText("Quality");

    if (ImGui::SliderInt("Sample Count", &julia.sampleCount, 100, 100000))
        ApplySettings();
    Tooltip("Number of points to sample (100-100000)");

    if (ImGui::SliderInt("Max Iterations", &julia.maxIterations, 10, 500))
        ApplySettings();
    Tooltip("Maximum iterations per point (10-500)");

    if (ImGui::SliderFloat("Escape Radius", &julia.escapeRadius, 1.5f, 10.0f, "%.2f"))
        ApplySettings();
    Tooltip("Escape threshold (1.5-10.0)");

    ImGui::SeparatorText("Rendering");

    if (ImGui::SliderFloat("Sampling Bounds", &julia.samplingBounds, 0.5f, 10.0f, "%.2f"))
        ApplySettings();
    Tooltip("4D space sampling bounds (0.5-10.0)");

    if (ImGui::SliderFloat("Render Size", &julia.renderSize, 0.1f, 5.0f, "%.2f"))
        ApplySettings();
    Tooltip("Overall size multiplier (0.1-5.0)");

    if (ImGui::SliderFloat("Point Size", &julia.pointSize, 0.5f, 20.0f, "%.1f"))
        ApplySettings();
    Tooltip("Individual point size (0.5-20.0)");

    ImGui::SeparatorText("Update Frequency");

    if (ImGui::Checkbox("Regenerate Continuously", &julia.regenerateContinuously))
        ApplySettings();
    Tooltip("Regenerate every frame (expensive!)");

    if (ImGui::SliderInt("Regenerate Interval", &julia.regenerateInterval, 1, 600))
        ApplySettings();
    Tooltip("Regenerate every N frames (1-600)");

    ImGui::SeparatorText("Audio Reactivity");

    if (ImGui::Checkbox("Audio Reactive Morph", &julia.audioReactiveMorph))
        ApplySettings();
    Tooltip("Morph c parameter with beats");

    if (ImGui::Checkbox("Audio Reactive Size", &julia.audioReactiveSize))
        ApplySettings();
    Tooltip("Scale with audio energy");

    // Reset button
    ImGui::Separator();
    if (ImGui::Button("Reset Julia Settings"))
    {
        m_settingsManager.ResetCategory(SettingsCategory::JuliaSet);
        ApplySettings();
    }
}

void SettingsUI::RenderFileButtons()
{
    if (ImGui::Button("Apply"))
    {
        ApplySettings();
    }
    ImGui::SameLine();

    if (ImGui::Button("Save to File"))
    {
        m_showSaveDialog = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Load from File"))
    {
        m_showLoadDialog = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Reset All"))
    {
        m_settingsManager.ResetToDefaults();
        ApplySettings();
    }
}

void SettingsUI::ApplySettings()
{
    // Validate settings
    m_settingsManager.ValidateSettings();

    // Trigger change notification
    m_settingsManager.NotifyChange(SettingsCategory::All);

    // Call apply callback if registered
    if (m_applyCallback)
    {
        m_applyCallback();
    }
}

void SettingsUI::Tooltip(const char* text)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip("%s", text);
    }
}

} // namespace Settings
} // namespace libprojectM
