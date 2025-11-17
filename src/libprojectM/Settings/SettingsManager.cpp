/**
 * @file SettingsManager.cpp
 * @brief Implementation of settings management
 */

#include "SettingsManager.hpp"

#include <fstream>
#include <sstream>

namespace libprojectM {
namespace Settings {

SettingsManager::SettingsManager()
{
    // Initialize with default values (already done by member initialization)
}

SettingsManager::SettingsManager(const std::string& configPath)
{
    LoadFromFile(configPath);
}

void SettingsManager::ValidateSettings()
{
    m_generalSettings.Validate();
    m_audioSettings.Validate();
    m_boidsSettings.Validate();
    m_rdSettings.Validate();
    m_hyper4DSettings.Validate();
    m_juliaSettings.Validate();
}

void SettingsManager::ValidateCategory(SettingsCategory category)
{
    switch (category)
    {
        case SettingsCategory::General:
            m_generalSettings.Validate();
            break;
        case SettingsCategory::Audio:
            m_audioSettings.Validate();
            break;
        case SettingsCategory::Boids:
            m_boidsSettings.Validate();
            break;
        case SettingsCategory::ReactionDiffusion:
            m_rdSettings.Validate();
            break;
        case SettingsCategory::Hyperdimensional4D:
            m_hyper4DSettings.Validate();
            break;
        case SettingsCategory::JuliaSet:
            m_juliaSettings.Validate();
            break;
        case SettingsCategory::All:
            ValidateSettings();
            break;
    }
}

bool SettingsManager::SaveToFile(const std::string& filePath) const
{
    try
    {
        std::string json = ToJSON();

        std::ofstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        file << json;
        file.close();

        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool SettingsManager::LoadFromFile(const std::string& filePath)
{
    try
    {
        std::ifstream file(filePath);
        if (!file.is_open())
        {
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string json = buffer.str();
        file.close();

        return FromJSON(json);
    }
    catch (...)
    {
        return false;
    }
}

void SettingsManager::ResetToDefaults()
{
    m_generalSettings.ResetToDefaults();
    m_audioSettings.ResetToDefaults();
    m_boidsSettings.ResetToDefaults();
    m_rdSettings.ResetToDefaults();
    m_hyper4DSettings.ResetToDefaults();
    m_juliaSettings.ResetToDefaults();

    NotifyChange(SettingsCategory::All);
}

void SettingsManager::ResetCategory(SettingsCategory category)
{
    switch (category)
    {
        case SettingsCategory::General:
            m_generalSettings.ResetToDefaults();
            break;
        case SettingsCategory::Audio:
            m_audioSettings.ResetToDefaults();
            break;
        case SettingsCategory::Boids:
            m_boidsSettings.ResetToDefaults();
            break;
        case SettingsCategory::ReactionDiffusion:
            m_rdSettings.ResetToDefaults();
            break;
        case SettingsCategory::Hyperdimensional4D:
            m_hyper4DSettings.ResetToDefaults();
            break;
        case SettingsCategory::JuliaSet:
            m_juliaSettings.ResetToDefaults();
            break;
        case SettingsCategory::All:
            ResetToDefaults();
            return;
    }

    NotifyChange(category);
}

void SettingsManager::RegisterChangeCallback(SettingsChangeCallback callback)
{
    m_changeCallbacks.push_back(callback);
}

void SettingsManager::NotifyChange(SettingsCategory category)
{
    for (const auto& callback : m_changeCallbacks)
    {
        callback(category);
    }
}

// Simple JSON serialization (can be replaced with proper library later)
std::string SettingsManager::ToJSON() const
{
    std::ostringstream json;

    json << "{\n";

    // General Settings
    json << "  \"general\": {\n";
    json << "    \"windowWidth\": " << m_generalSettings.windowWidth << ",\n";
    json << "    \"windowHeight\": " << m_generalSettings.windowHeight << ",\n";
    json << "    \"fullscreen\": " << (m_generalSettings.fullscreen ? "true" : "false") << ",\n";
    json << "    \"vsync\": " << (m_generalSettings.vsync ? "true" : "false") << ",\n";
    json << "    \"fps\": " << m_generalSettings.fps << ",\n";
    json << "    \"presetDuration\": " << m_generalSettings.presetDuration << "\n";
    json << "  },\n";

    // Audio Settings
    json << "  \"audio\": {\n";
    json << "    \"beatSensitivity\": " << m_audioSettings.beatSensitivity << ",\n";
    json << "    \"volumeBoost\": " << m_audioSettings.volumeBoost << ",\n";
    json << "    \"enableBeatDetection\": " << (m_audioSettings.enableBeatDetection ? "true" : "false") << "\n";
    json << "  },\n";

    // Boids Settings
    json << "  \"boids\": {\n";
    json << "    \"enabled\": " << (m_boidsSettings.enabled ? "true" : "false") << ",\n";
    json << "    \"maxBoids\": " << m_boidsSettings.maxBoids << ",\n";
    json << "    \"separationWeight\": " << m_boidsSettings.separationWeight << ",\n";
    json << "    \"alignmentWeight\": " << m_boidsSettings.alignmentWeight << ",\n";
    json << "    \"cohesionWeight\": " << m_boidsSettings.cohesionWeight << "\n";
    json << "  },\n";

    // Reaction-Diffusion Settings
    json << "  \"reactionDiffusion\": {\n";
    json << "    \"enabled\": " << (m_rdSettings.enabled ? "true" : "false") << ",\n";
    json << "    \"gridWidth\": " << m_rdSettings.gridWidth << ",\n";
    json << "    \"gridHeight\": " << m_rdSettings.gridHeight << ",\n";
    json << "    \"preset\": " << static_cast<int>(m_rdSettings.preset) << "\n";
    json << "  },\n";

    // 4D Settings
    json << "  \"hyperdimensional4D\": {\n";
    json << "    \"enabled\": " << (m_hyper4DSettings.enabled ? "true" : "false") << ",\n";
    json << "    \"tesseractSize\": " << m_hyper4DSettings.tesseractSize << ",\n";
    json << "    \"cameraDistance4D\": " << m_hyper4DSettings.cameraDistance4D << "\n";
    json << "  },\n";

    // Julia Set Settings
    json << "  \"juliaSet\": {\n";
    json << "    \"enabled\": " << (m_juliaSettings.enabled ? "true" : "false") << ",\n";
    json << "    \"preset\": " << static_cast<int>(m_juliaSettings.preset) << ",\n";
    json << "    \"sampleCount\": " << m_juliaSettings.sampleCount << ",\n";
    json << "    \"maxIterations\": " << m_juliaSettings.maxIterations << "\n";
    json << "  }\n";

    json << "}\n";

    return json.str();
}

bool SettingsManager::FromJSON(const std::string& json)
{
    // Simple JSON parsing (this is a minimal implementation)
    // In a real implementation, use a proper JSON library

    auto getValue = [&json](const std::string& key) -> std::string {
        size_t keyPos = json.find("\"" + key + "\"");
        if (keyPos == std::string::npos)
            return "";

        size_t colonPos = json.find(":", keyPos);
        if (colonPos == std::string::npos)
            return "";

        size_t valueStart = colonPos + 1;
        while (valueStart < json.length() && std::isspace(json[valueStart]))
            valueStart++;

        size_t valueEnd = json.find_first_of(",\n}", valueStart);
        if (valueEnd == std::string::npos)
            return "";

        std::string value = json.substr(valueStart, valueEnd - valueStart);

        // Trim whitespace
        size_t start = value.find_first_not_of(" \t\r\n");
        size_t end = value.find_last_not_of(" \t\r\n");

        if (start == std::string::npos)
            return "";

        return value.substr(start, end - start + 1);
    };

    auto getInt = [&getValue](const std::string& key, int defaultValue) -> int {
        std::string value = getValue(key);
        if (value.empty())
            return defaultValue;
        try
        {
            return std::stoi(value);
        }
        catch (...)
        {
            return defaultValue;
        }
    };

    auto getFloat = [&getValue](const std::string& key, float defaultValue) -> float {
        std::string value = getValue(key);
        if (value.empty())
            return defaultValue;
        try
        {
            return std::stof(value);
        }
        catch (...)
        {
            return defaultValue;
        }
    };

    auto getBool = [&getValue](const std::string& key, bool defaultValue) -> bool {
        std::string value = getValue(key);
        if (value.empty())
            return defaultValue;
        return value == "true" || value == "1";
    };

    // Parse general settings
    m_generalSettings.windowWidth = getInt("windowWidth", 1920);
    m_generalSettings.windowHeight = getInt("windowHeight", 1080);
    m_generalSettings.fullscreen = getBool("fullscreen", false);
    m_generalSettings.vsync = getBool("vsync", true);
    m_generalSettings.fps = getInt("fps", 60);
    m_generalSettings.presetDuration = getInt("presetDuration", 10);

    // Parse audio settings
    m_audioSettings.beatSensitivity = getFloat("beatSensitivity", 1.0f);
    m_audioSettings.volumeBoost = getFloat("volumeBoost", 1.0f);
    m_audioSettings.enableBeatDetection = getBool("enableBeatDetection", true);

    // Parse boids settings
    m_boidsSettings.enabled = getBool("boids.enabled", true);
    m_boidsSettings.maxBoids = getInt("maxBoids", 1000);
    m_boidsSettings.separationWeight = getFloat("separationWeight", 1.5f);
    m_boidsSettings.alignmentWeight = getFloat("alignmentWeight", 1.0f);
    m_boidsSettings.cohesionWeight = getFloat("cohesionWeight", 1.0f);

    // Parse RD settings
    m_rdSettings.enabled = getBool("reactionDiffusion.enabled", true);
    m_rdSettings.gridWidth = getInt("gridWidth", 128);
    m_rdSettings.gridHeight = getInt("gridHeight", 128);

    // Parse 4D settings
    m_hyper4DSettings.enabled = getBool("hyperdimensional4D.enabled", true);
    m_hyper4DSettings.tesseractSize = getFloat("tesseractSize", 2.0f);
    m_hyper4DSettings.cameraDistance4D = getFloat("cameraDistance4D", 8.0f);

    // Parse Julia settings
    m_juliaSettings.enabled = getBool("juliaSet.enabled", true);
    m_juliaSettings.sampleCount = getInt("sampleCount", 5000);
    m_juliaSettings.maxIterations = getInt("maxIterations", 100);

    // Validate all loaded settings
    ValidateSettings();

    return true;
}

} // namespace Settings
} // namespace libprojectM
