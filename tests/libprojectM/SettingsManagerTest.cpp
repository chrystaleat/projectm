/**
 * @file SettingsManagerTest.cpp
 * @brief Test suite for comprehensive settings management system
 */

#include <gtest/gtest.h>

#include <Settings/SettingsManager.hpp>
#include <Settings/GeneralSettings.hpp>
#include <Settings/AudioSettings.hpp>
#include <Settings/BoidsSettings.hpp>
#include <Settings/ReactionDiffusionSettings.hpp>
#include <Settings/Hyperdimensional4DSettings.hpp>
#include <Settings/JuliaSetSettings.hpp>

#include <fstream>
#include <filesystem>

using namespace libprojectM::Settings;

class SettingsManagerTest : public ::testing::Test
{
protected:
    static constexpr float EPSILON = 0.001f;
    const std::string testConfigPath = "/tmp/projectm_test_settings.json";

    void SetUp() override
    {
        // Clean up any previous test config
        if (std::filesystem::exists(testConfigPath))
        {
            std::filesystem::remove(testConfigPath);
        }

        settingsManager = std::make_unique<SettingsManager>();
    }

    void TearDown() override
    {
        // Clean up test config
        if (std::filesystem::exists(testConfigPath))
        {
            std::filesystem::remove(testConfigPath);
        }
    }

    std::unique_ptr<SettingsManager> settingsManager;
};

// ====== General Settings Tests ======

TEST_F(SettingsManagerTest, GeneralSettings_DefaultValues)
{
    const auto& general = settingsManager->GetGeneralSettings();

    EXPECT_EQ(general.fps, 60);
    EXPECT_EQ(general.windowWidth, 1920);
    EXPECT_EQ(general.windowHeight, 1080);
    EXPECT_FALSE(general.fullscreen);
    EXPECT_TRUE(general.vsync);
}

TEST_F(SettingsManagerTest, GeneralSettings_Modification)
{
    auto& general = settingsManager->GetGeneralSettings();

    general.fps = 120;
    general.fullscreen = true;

    EXPECT_EQ(general.fps, 120);
    EXPECT_TRUE(general.fullscreen);
}

TEST_F(SettingsManagerTest, GeneralSettings_Validation)
{
    auto& general = settingsManager->GetGeneralSettings();

    // FPS should be clamped to valid range
    general.fps = 300;
    settingsManager->ValidateSettings();

    EXPECT_LE(general.fps, 240); // Max FPS
}

// ====== Audio Settings Tests ======

TEST_F(SettingsManagerTest, AudioSettings_DefaultValues)
{
    const auto& audio = settingsManager->GetAudioSettings();

    EXPECT_FLOAT_EQ(audio.beatSensitivity, 1.0f);
    EXPECT_FLOAT_EQ(audio.volumeBoost, 1.0f);
    EXPECT_TRUE(audio.enableBeatDetection);
}

TEST_F(SettingsManagerTest, AudioSettings_Modification)
{
    auto& audio = settingsManager->GetAudioSettings();

    audio.beatSensitivity = 2.5f;
    audio.enableBeatDetection = false;

    EXPECT_FLOAT_EQ(audio.beatSensitivity, 2.5f);
    EXPECT_FALSE(audio.enableBeatDetection);
}

// ====== Boids Settings Tests ======

TEST_F(SettingsManagerTest, BoidsSettings_DefaultValues)
{
    const auto& boids = settingsManager->GetBoidsSettings();

    EXPECT_TRUE(boids.enabled);
    EXPECT_EQ(boids.maxBoids, 1000);
    EXPECT_FLOAT_EQ(boids.separationWeight, 1.5f);
    EXPECT_FLOAT_EQ(boids.alignmentWeight, 1.0f);
    EXPECT_FLOAT_EQ(boids.cohesionWeight, 1.0f);
}

TEST_F(SettingsManagerTest, BoidsSettings_Presets)
{
    auto& boids = settingsManager->GetBoidsSettings();

    // Apply "Scattered" preset
    boids.ApplyPreset(BoidsPreset::Scattered);

    EXPECT_GT(boids.separationWeight, boids.cohesionWeight); // More separation

    // Apply "Tight" preset
    boids.ApplyPreset(BoidsPreset::Tight);

    EXPECT_GT(boids.alignmentWeight, 1.5f); // High alignment
}

TEST_F(SettingsManagerTest, BoidsSettings_AudioReactivity)
{
    auto& boids = settingsManager->GetBoidsSettings();

    boids.audioReactiveSpeed = true;
    boids.audioReactiveSeparation = true;

    EXPECT_TRUE(boids.audioReactiveSpeed);
    EXPECT_TRUE(boids.audioReactiveSeparation);
}

// ====== Reaction-Diffusion Settings Tests ======

TEST_F(SettingsManagerTest, ReactionDiffusionSettings_DefaultValues)
{
    const auto& rd = settingsManager->GetReactionDiffusionSettings();

    EXPECT_TRUE(rd.enabled);
    EXPECT_EQ(rd.gridWidth, 128);
    EXPECT_EQ(rd.gridHeight, 128);
    EXPECT_EQ(rd.preset, RDPreset::Spots);
}

TEST_F(SettingsManagerTest, ReactionDiffusionSettings_Presets)
{
    auto& rd = settingsManager->GetReactionDiffusionSettings();

    rd.preset = RDPreset::Stripes;

    EXPECT_EQ(rd.preset, RDPreset::Stripes);
    EXPECT_FLOAT_EQ(rd.feedRate, 0.035f);  // Stripes F value
}

TEST_F(SettingsManagerTest, ReactionDiffusionSettings_GridSize)
{
    auto& rd = settingsManager->GetReactionDiffusionSettings();

    rd.gridWidth = 256;
    rd.gridHeight = 256;

    EXPECT_EQ(rd.gridWidth, 256);
    EXPECT_EQ(rd.gridHeight, 256);
}

// ====== 4D Hyperdimensional Settings Tests ======

TEST_F(SettingsManagerTest, Hyperdimensional4DSettings_DefaultValues)
{
    const auto& hyper4d = settingsManager->GetHyperdimensional4DSettings();

    EXPECT_TRUE(hyper4d.enabled);
    EXPECT_FLOAT_EQ(hyper4d.tesseractSize, 2.0f);
    EXPECT_FLOAT_EQ(hyper4d.cameraDistance4D, 8.0f);
}

TEST_F(SettingsManagerTest, Hyperdimensional4DSettings_RotationSpeeds)
{
    auto& hyper4d = settingsManager->GetHyperdimensional4DSettings();

    hyper4d.rotationSpeedXY = 0.05f;
    hyper4d.rotationSpeedZW = 0.03f;

    EXPECT_FLOAT_EQ(hyper4d.rotationSpeedXY, 0.05f);
    EXPECT_FLOAT_EQ(hyper4d.rotationSpeedZW, 0.03f);
}

TEST_F(SettingsManagerTest, Hyperdimensional4DSettings_AudioReactive)
{
    auto& hyper4d = settingsManager->GetHyperdimensional4DSettings();

    hyper4d.audioReactiveRotation = true;
    hyper4d.audioReactiveScale = true;

    EXPECT_TRUE(hyper4d.audioReactiveRotation);
    EXPECT_TRUE(hyper4d.audioReactiveScale);
}

// ====== Julia Set Settings Tests ======

TEST_F(SettingsManagerTest, JuliaSetSettings_DefaultValues)
{
    const auto& julia = settingsManager->GetJuliaSetSettings();

    EXPECT_TRUE(julia.enabled);
    EXPECT_EQ(julia.preset, JuliaPreset::Classic);
    EXPECT_EQ(julia.sampleCount, 5000);
    EXPECT_EQ(julia.maxIterations, 100);
}

TEST_F(SettingsManagerTest, JuliaSetSettings_Presets)
{
    auto& julia = settingsManager->GetJuliaSetSettings();

    julia.preset = JuliaPreset::Spiral;

    EXPECT_EQ(julia.preset, JuliaPreset::Spiral);
}

TEST_F(SettingsManagerTest, JuliaSetSettings_Quality)
{
    auto& julia = settingsManager->GetJuliaSetSettings();

    julia.sampleCount = 10000;
    julia.maxIterations = 150;

    EXPECT_EQ(julia.sampleCount, 10000);
    EXPECT_EQ(julia.maxIterations, 150);
}

// ====== Persistence Tests ======

TEST_F(SettingsManagerTest, SaveSettings)
{
    auto& general = settingsManager->GetGeneralSettings();
    general.fps = 90;
    general.fullscreen = true;

    auto& boids = settingsManager->GetBoidsSettings();
    boids.maxBoids = 2000;

    // Save to file
    bool success = settingsManager->SaveToFile(testConfigPath);

    EXPECT_TRUE(success);
    EXPECT_TRUE(std::filesystem::exists(testConfigPath));
}

TEST_F(SettingsManagerTest, LoadSettings)
{
    // First save settings
    auto& general = settingsManager->GetGeneralSettings();
    general.fps = 90;

    auto& boids = settingsManager->GetBoidsSettings();
    boids.maxBoids = 2000;

    settingsManager->SaveToFile(testConfigPath);

    // Create new manager and load
    auto newManager = std::make_unique<SettingsManager>();
    bool success = newManager->LoadFromFile(testConfigPath);

    EXPECT_TRUE(success);
    EXPECT_EQ(newManager->GetGeneralSettings().fps, 90);
    EXPECT_EQ(newManager->GetBoidsSettings().maxBoids, 2000);
}

TEST_F(SettingsManagerTest, LoadSettings_InvalidFile)
{
    bool success = settingsManager->LoadFromFile("/nonexistent/path.json");

    EXPECT_FALSE(success);
    // Should still have default values
    EXPECT_EQ(settingsManager->GetGeneralSettings().fps, 60);
}

// ====== Reset Tests ======

TEST_F(SettingsManagerTest, ResetToDefaults)
{
    // Modify settings
    auto& general = settingsManager->GetGeneralSettings();
    general.fps = 120;

    auto& boids = settingsManager->GetBoidsSettings();
    boids.maxBoids = 3000;

    // Reset
    settingsManager->ResetToDefaults();

    EXPECT_EQ(settingsManager->GetGeneralSettings().fps, 60);
    EXPECT_EQ(settingsManager->GetBoidsSettings().maxBoids, 1000);
}

TEST_F(SettingsManagerTest, ResetCategory)
{
    // Modify boids
    auto& boids = settingsManager->GetBoidsSettings();
    boids.maxBoids = 3000;
    boids.separationWeight = 5.0f;

    // Reset just boids
    settingsManager->ResetCategory(SettingsCategory::Boids);

    EXPECT_EQ(boids.maxBoids, 1000);
    EXPECT_FLOAT_EQ(boids.separationWeight, 1.5f);
}

// ====== Change Notification Tests ======

TEST_F(SettingsManagerTest, ChangeNotification)
{
    bool notified = false;
    SettingsCategory changedCategory;

    settingsManager->RegisterChangeCallback([&](SettingsCategory category) {
        notified = true;
        changedCategory = category;
    });

    // Modify a setting
    auto& general = settingsManager->GetGeneralSettings();
    general.fps = 120;

    settingsManager->NotifyChange(SettingsCategory::General);

    EXPECT_TRUE(notified);
    EXPECT_EQ(changedCategory, SettingsCategory::General);
}

// ====== Validation Tests ======

TEST_F(SettingsManagerTest, Validation_ClampValues)
{
    auto& general = settingsManager->GetGeneralSettings();

    // Set invalid values
    general.fps = 500;  // Too high
    general.windowWidth = 0;  // Too low

    settingsManager->ValidateSettings();

    EXPECT_LE(general.fps, 240);  // Clamped
    EXPECT_GE(general.windowWidth, 320);  // Minimum
}

TEST_F(SettingsManagerTest, Validation_GridSizePowerOfTwo)
{
    auto& rd = settingsManager->GetReactionDiffusionSettings();

    rd.gridWidth = 100;  // Not power of 2

    settingsManager->ValidateSettings();

    // Should be adjusted to nearest power of 2 (128)
    EXPECT_TRUE(IsPowerOfTwo(rd.gridWidth));
}

// ====== Helper Methods ======

protected:
    bool IsPowerOfTwo(int value)
    {
        return value > 0 && (value & (value - 1)) == 0;
    }
};
