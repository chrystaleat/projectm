/**
 * @file SettingsManager.cpp
 * @brief Implementation of settings manager
 */

#include "SettingsManager.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>

namespace libprojectM {
namespace Settings {

SettingsManager::SettingsManager()
    : m_changeCallback(nullptr)
{
    // Settings are initialized with default values in the header
}

SettingsManager::~SettingsManager() = default;

void SettingsManager::RegisterChangeCallback(ChangeCallback callback)
{
    m_changeCallback = callback;
}

void SettingsManager::NotifyChange(SettingsCategory category)
{
    if (m_changeCallback)
    {
        m_changeCallback(category);
    }
}

bool SettingsManager::SaveToFile(const std::string& filepath)
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        return false;
    }

    file << std::fixed << std::setprecision(6);

    // Save Boids settings
    file << "# Boids Flocking Settings\n";
    file << "Boids.MaxBoids = " << m_boidsSettings.maxBoids << "\n";
    file << "Boids.SpawnCount = " << m_boidsSettings.spawnCount << "\n";
    file << "Boids.SeparationWeight = " << m_boidsSettings.separationWeight << "\n";
    file << "Boids.AlignmentWeight = " << m_boidsSettings.alignmentWeight << "\n";
    file << "Boids.CohesionWeight = " << m_boidsSettings.cohesionWeight << "\n";
    file << "Boids.MaxSpeed = " << m_boidsSettings.maxSpeed << "\n";
    file << "Boids.MaxForce = " << m_boidsSettings.maxForce << "\n";
    file << "Boids.SeparationRadius = " << m_boidsSettings.separationRadius << "\n";
    file << "Boids.AlignmentRadius = " << m_boidsSettings.alignmentRadius << "\n";
    file << "Boids.CohesionRadius = " << m_boidsSettings.cohesionRadius << "\n";
    file << "Boids.AudioReactiveSeparation = " << m_boidsSettings.audioReactiveSeparation << "\n";
    file << "Boids.AudioReactiveSpeed = " << m_boidsSettings.audioReactiveSpeed << "\n";
    file << "Boids.AudioReactiveSpawn = " << m_boidsSettings.audioReactiveSpawn << "\n";
    file << "\n";

    // Save Reaction-Diffusion settings
    file << "# Reaction-Diffusion Settings\n";
    file << "RD.GridWidth = " << m_rdSettings.gridWidth << "\n";
    file << "RD.GridHeight = " << m_rdSettings.gridHeight << "\n";
    file << "RD.FeedRate = " << m_rdSettings.feedRate << "\n";
    file << "RD.KillRate = " << m_rdSettings.killRate << "\n";
    file << "RD.DiffusionU = " << m_rdSettings.diffusionU << "\n";
    file << "RD.DiffusionV = " << m_rdSettings.diffusionV << "\n";
    file << "RD.Timestep = " << m_rdSettings.timestep << "\n";
    file << "RD.IterationsPerFrame = " << m_rdSettings.iterationsPerFrame << "\n";
    file << "RD.AutoSeed = " << m_rdSettings.autoSeed << "\n";
    file << "RD.AudioReactiveFeedRate = " << m_rdSettings.audioReactiveFeedRate << "\n";
    file << "RD.AudioReactiveKillRate = " << m_rdSettings.audioReactiveKillRate << "\n";
    file << "RD.AudioReactiveDiffusion = " << m_rdSettings.audioReactiveDiffusion << "\n";
    file << "\n";

    // Save 4D settings
    file << "# 4D Hyperdimensional Visualization Settings\n";
    file << "4D.TesseractSize = " << m_4dSettings.tesseractSize << "\n";
    file << "4D.EdgeThickness = " << m_4dSettings.edgeThickness << "\n";
    file << "4D.RenderMode = " << m_4dSettings.renderMode << "\n";
    file << "4D.Camera4DDistance = " << m_4dSettings.camera4DDistance << "\n";
    file << "4D.Camera4DFOV = " << m_4dSettings.camera4DFOV << "\n";
    file << "4D.RotationXY = " << m_4dSettings.rotationXY << "\n";
    file << "4D.RotationZW = " << m_4dSettings.rotationZW << "\n";
    file << "4D.RotationXZ = " << m_4dSettings.rotationXZ << "\n";
    file << "4D.RotationYW = " << m_4dSettings.rotationYW << "\n";
    file << "4D.VertexSize = " << m_4dSettings.vertexSize << "\n";
    file << "4D.ColorCycleSpeed = " << m_4dSettings.colorCycleSpeed << "\n";
    file << "4D.PerspectiveProjection = " << m_4dSettings.perspectiveProjection << "\n";
    file << "4D.AudioReactiveRotation = " << m_4dSettings.audioReactiveRotation << "\n";
    file << "\n";

    // Save Julia Set settings
    file << "# Julia Set Fractal Settings\n";
    file << "Julia.C_A = " << m_juliaSetSettings.c_a << "\n";
    file << "Julia.C_B = " << m_juliaSetSettings.c_b << "\n";
    file << "Julia.C_C = " << m_juliaSetSettings.c_c << "\n";
    file << "Julia.C_D = " << m_juliaSetSettings.c_d << "\n";
    file << "Julia.MaxIterations = " << m_juliaSetSettings.maxIterations << "\n";
    file << "Julia.SampleCount = " << m_juliaSetSettings.sampleCount << "\n";
    file << "Julia.EscapeRadius = " << m_juliaSetSettings.escapeRadius << "\n";
    file << "Julia.PointSize = " << m_juliaSetSettings.pointSize << "\n";
    file << "Julia.MinBound = " << m_juliaSetSettings.minBound << "\n";
    file << "Julia.MaxBound = " << m_juliaSetSettings.maxBound << "\n";
    file << "Julia.CParameterSpeed = " << m_juliaSetSettings.cParameterSpeed << "\n";
    file << "Julia.AutoRotate = " << m_juliaSetSettings.autoRotate << "\n";
    file << "Julia.AudioReactiveCParameter = " << m_juliaSetSettings.audioReactiveCParameter << "\n";
    file << "Julia.AudioReactiveIterations = " << m_juliaSetSettings.audioReactiveIterations << "\n";

    file.close();
    return true;
}

bool SettingsManager::LoadFromFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        // Parse key = value
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos)
        {
            continue;
        }

        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        // Parse boids settings
        if (key == "Boids.MaxBoids") m_boidsSettings.maxBoids = std::stoi(value);
        else if (key == "Boids.SpawnCount") m_boidsSettings.spawnCount = std::stoi(value);
        else if (key == "Boids.SeparationWeight") m_boidsSettings.separationWeight = std::stof(value);
        else if (key == "Boids.AlignmentWeight") m_boidsSettings.alignmentWeight = std::stof(value);
        else if (key == "Boids.CohesionWeight") m_boidsSettings.cohesionWeight = std::stof(value);
        else if (key == "Boids.MaxSpeed") m_boidsSettings.maxSpeed = std::stof(value);
        else if (key == "Boids.MaxForce") m_boidsSettings.maxForce = std::stof(value);
        else if (key == "Boids.SeparationRadius") m_boidsSettings.separationRadius = std::stof(value);
        else if (key == "Boids.AlignmentRadius") m_boidsSettings.alignmentRadius = std::stof(value);
        else if (key == "Boids.CohesionRadius") m_boidsSettings.cohesionRadius = std::stof(value);
        else if (key == "Boids.AudioReactiveSeparation") m_boidsSettings.audioReactiveSeparation = (value == "1" || value == "true");
        else if (key == "Boids.AudioReactiveSpeed") m_boidsSettings.audioReactiveSpeed = (value == "1" || value == "true");
        else if (key == "Boids.AudioReactiveSpawn") m_boidsSettings.audioReactiveSpawn = (value == "1" || value == "true");

        // Parse RD settings
        else if (key == "RD.GridWidth") m_rdSettings.gridWidth = std::stoi(value);
        else if (key == "RD.GridHeight") m_rdSettings.gridHeight = std::stoi(value);
        else if (key == "RD.FeedRate") m_rdSettings.feedRate = std::stof(value);
        else if (key == "RD.KillRate") m_rdSettings.killRate = std::stof(value);
        else if (key == "RD.DiffusionU") m_rdSettings.diffusionU = std::stof(value);
        else if (key == "RD.DiffusionV") m_rdSettings.diffusionV = std::stof(value);
        else if (key == "RD.Timestep") m_rdSettings.timestep = std::stof(value);
        else if (key == "RD.IterationsPerFrame") m_rdSettings.iterationsPerFrame = std::stoi(value);
        else if (key == "RD.AutoSeed") m_rdSettings.autoSeed = (value == "1" || value == "true");
        else if (key == "RD.AudioReactiveFeedRate") m_rdSettings.audioReactiveFeedRate = (value == "1" || value == "true");
        else if (key == "RD.AudioReactiveKillRate") m_rdSettings.audioReactiveKillRate = (value == "1" || value == "true");
        else if (key == "RD.AudioReactiveDiffusion") m_rdSettings.audioReactiveDiffusion = (value == "1" || value == "true");

        // Parse 4D settings
        else if (key == "4D.TesseractSize") m_4dSettings.tesseractSize = std::stof(value);
        else if (key == "4D.EdgeThickness") m_4dSettings.edgeThickness = std::stof(value);
        else if (key == "4D.RenderMode") m_4dSettings.renderMode = std::stoi(value);
        else if (key == "4D.Camera4DDistance") m_4dSettings.camera4DDistance = std::stof(value);
        else if (key == "4D.Camera4DFOV") m_4dSettings.camera4DFOV = std::stof(value);
        else if (key == "4D.RotationXY") m_4dSettings.rotationXY = std::stof(value);
        else if (key == "4D.RotationZW") m_4dSettings.rotationZW = std::stof(value);
        else if (key == "4D.RotationXZ") m_4dSettings.rotationXZ = std::stof(value);
        else if (key == "4D.RotationYW") m_4dSettings.rotationYW = std::stof(value);
        else if (key == "4D.VertexSize") m_4dSettings.vertexSize = std::stof(value);
        else if (key == "4D.ColorCycleSpeed") m_4dSettings.colorCycleSpeed = std::stof(value);
        else if (key == "4D.PerspectiveProjection") m_4dSettings.perspectiveProjection = (value == "1" || value == "true");
        else if (key == "4D.AudioReactiveRotation") m_4dSettings.audioReactiveRotation = (value == "1" || value == "true");

        // Parse Julia settings
        else if (key == "Julia.C_A") m_juliaSetSettings.c_a = std::stof(value);
        else if (key == "Julia.C_B") m_juliaSetSettings.c_b = std::stof(value);
        else if (key == "Julia.C_C") m_juliaSetSettings.c_c = std::stof(value);
        else if (key == "Julia.C_D") m_juliaSetSettings.c_d = std::stof(value);
        else if (key == "Julia.MaxIterations") m_juliaSetSettings.maxIterations = std::stoi(value);
        else if (key == "Julia.SampleCount") m_juliaSetSettings.sampleCount = std::stoi(value);
        else if (key == "Julia.EscapeRadius") m_juliaSetSettings.escapeRadius = std::stof(value);
        else if (key == "Julia.PointSize") m_juliaSetSettings.pointSize = std::stof(value);
        else if (key == "Julia.MinBound") m_juliaSetSettings.minBound = std::stof(value);
        else if (key == "Julia.MaxBound") m_juliaSetSettings.maxBound = std::stof(value);
        else if (key == "Julia.CParameterSpeed") m_juliaSetSettings.cParameterSpeed = std::stof(value);
        else if (key == "Julia.AutoRotate") m_juliaSetSettings.autoRotate = (value == "1" || value == "true");
        else if (key == "Julia.AudioReactiveCParameter") m_juliaSetSettings.audioReactiveCParameter = (value == "1" || value == "true");
        else if (key == "Julia.AudioReactiveIterations") m_juliaSetSettings.audioReactiveIterations = (value == "1" || value == "true");
    }

    file.close();
    return true;
}

void SettingsManager::ResetToDefaults(SettingsCategory category)
{
    switch (category)
    {
        case SettingsCategory::Boids:
            m_boidsSettings = BoidsSettings();
            break;
        case SettingsCategory::ReactionDiffusion:
            m_rdSettings = ReactionDiffusionSettings();
            break;
        case SettingsCategory::Hyperdimensional4D:
            m_4dSettings = Hyperdimensional4DSettings();
            break;
        case SettingsCategory::JuliaSet:
            m_juliaSetSettings = JuliaSetSettings();
            break;
        default:
            break;
    }

    NotifyChange(category);
}

void SettingsManager::ResetAll()
{
    m_boidsSettings = BoidsSettings();
    m_rdSettings = ReactionDiffusionSettings();
    m_4dSettings = Hyperdimensional4DSettings();
    m_juliaSetSettings = JuliaSetSettings();

    NotifyChange(SettingsCategory::Boids);
    NotifyChange(SettingsCategory::ReactionDiffusion);
    NotifyChange(SettingsCategory::Hyperdimensional4D);
    NotifyChange(SettingsCategory::JuliaSet);
}

} // namespace Settings
} // namespace libprojectM
