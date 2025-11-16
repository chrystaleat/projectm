# 🚀 New Features Integration Guide

This document explains how to integrate the new visualization features into projectM's rendering pipeline.

---

## 📋 Table of Contents

1. [Quick Start](#quick-start)
2. [Boids Flocking Integration](#boids-flocking-integration)
3. [Reaction-Diffusion Integration](#reaction-diffusion-integration)
4. [4D Visualization Integration](#4d-visualization-integration)
5. [Quaternion Julia Sets Integration](#quaternion-julia-sets-integration)
6. [Combining Features](#combining-features)
7. [Performance Optimization](#performance-optimization)

---

## 🎯 Quick Start

All new features are in the `libprojectM::Renderer` namespace and are header-only or have compiled implementations in the Renderer library.

### Include Headers

```cpp
#include <Renderer/BoidsSystem.hpp>
#include <Renderer/ReactionDiffusion.hpp>
#include <Renderer/Tesseract.hpp>
#include <Renderer/HyperdimensionalProjection.hpp>
#include <Renderer/QuaternionJulia.hpp>
```

### Link Library

Already linked if you're building projectM! The features are part of the `Renderer` object library.

---

## 🐦 Boids Flocking Integration

### Basic Usage

```cpp
class BoidsVisualizer
{
public:
    BoidsVisualizer()
        : m_boids(1000)  // Max 1000 boids
    {
        // Configure behavior
        m_boids.SetSeparationWeight(1.5f);
        m_boids.SetAlignmentWeight(1.0f);
        m_boids.SetCohesionWeight(1.0f);
        m_boids.SetPerceptionRadius(3.0f);
        m_boids.SetMaxSpeed(5.0f);

        // Set world bounds
        m_boids.SetBoundingBox(-50, -50, -50, 50, 50, 50);
        m_boids.SetBoundaryMode(BoundaryMode::Wrap);

        // Spawn initial boids
        SpawnBoids(500);
    }

    void Update(const AudioFeatures& audio, float deltaTime)
    {
        // Audio reactivity
        float energyMultiplier = 1.0f + audio.energy * 0.5f;
        m_boids.SetAudioEnergyMultiplier(energyMultiplier);

        // Beat detection -> separation pulse
        if (audio.onsetDetected)
        {
            m_boids.SetBeatIntensity(1.0f);
        }

        // Update simulation
        m_boids.Update(deltaTime);
    }

    void Render(RenderContext& ctx)
    {
        const auto& boids = m_boids.GetBoids();

        // Collect active boid positions
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> velocities;

        for (const auto* boid : boids)
        {
            if (boid && boid->active)
            {
                positions.push_back(boid->position);
                velocities.push_back(boid->GetVelocity());
            }
        }

        // Render as point sprites
        RenderAsPointCloud(ctx, positions, velocities);
    }

private:
    BoidsSystem m_boids;

    void SpawnBoids(int count)
    {
        for (int i = 0; i < count; ++i)
        {
            glm::vec3 pos = RandomPosition();
            glm::vec3 vel = RandomVelocity();
            m_boids.AddBoid(pos, vel);
        }
    }

    void RenderAsPointCloud(RenderContext& ctx,
                           const std::vector<glm::vec3>& positions,
                           const std::vector<glm::vec3>& velocities)
    {
        // Upload to GPU
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     positions.size() * sizeof(glm::vec3),
                     positions.data(), GL_DYNAMIC_DRAW);

        // Bind shader
        m_shader.Bind();
        m_shader.SetUniform("projectionMatrix", ctx.projectionMatrix);
        m_shader.SetUniform("viewMatrix", ctx.viewMatrix);

        // Render
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, positions.size());
    }
};
```

### Shader Example (Point Sprites)

```glsl
// Vertex Shader
#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragColor;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * vec4(position, 1.0);
    gl_PointSize = 4.0;  // Or calculate based on depth

    // Color based on position or velocity
    fragColor = vec3(0.5) + position * 0.1;
}

// Fragment Shader
#version 330 core
in vec3 fragColor;
out vec4 outColor;

void main()
{
    // Circular point sprite
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    if (dist > 0.5)
        discard;

    float alpha = 1.0 - (dist * 2.0);
    outColor = vec4(fragColor, alpha);
}
```

---

## 🧪 Reaction-Diffusion Integration

### Basic Usage

```cpp
class ReactionDiffusionVisualizer
{
public:
    ReactionDiffusionVisualizer()
        : m_rd(256, 256)  // 256x256 grid
    {
        // Choose pattern
        m_rd.SetPreset(ReactionDiffusionPreset::Spots);

        // Seed initial pattern
        m_rd.SeedRandom(10, 5);

        // Create OpenGL texture
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Update(const AudioFeatures& audio, float deltaTime)
    {
        // Audio modulation
        m_rd.SetAudioModulation(audio.energy, audio.onsetDetected ? 1.0f : 0.0f, audio.bass);

        // Simulate
        m_rd.Update(deltaTime);

        // Update texture every N frames (optimization)
        if (++m_frameCount % 2 == 0)
        {
            UpdateTexture();
        }
    }

    void Render(RenderContext& ctx)
    {
        // Bind texture
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // Render full-screen quad with texture
        RenderFullScreenQuad(ctx);

        // Or blend with other elements
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive
        // RenderOverlay(ctx);
    }

private:
    ReactionDiffusion m_rd;
    GLuint m_texture;
    int m_frameCount = 0;

    void UpdateTexture()
    {
        auto data = m_rd.GetTextureData();

        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     m_rd.GetWidth(), m_rd.GetHeight(),
                     0, GL_RGB, GL_FLOAT, data.data());
    }

    void RenderFullScreenQuad(RenderContext& ctx)
    {
        // Bind shader
        m_shader.Bind();
        m_shader.SetUniform("rdTexture", 0);

        // Draw quad
        glBindVertexArray(m_quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
};
```

### Fragment Shader (Color Mapping)

```glsl
#version 330 core
in vec2 texCoord;
out vec4 outColor;

uniform sampler2D rdTexture;

void main()
{
    float v = texture(rdTexture, texCoord).r;  // V concentration

    // Color mapping
    vec3 color = vec3(0.0);

    if (v < 0.3)
    {
        // Background
        color = vec3(0.0, 0.0, 0.1);
    }
    else if (v < 0.7)
    {
        // Transition
        color = mix(vec3(0.0, 0.2, 0.5), vec3(0.5, 0.8, 1.0), (v - 0.3) / 0.4);
    }
    else
    {
        // Pattern
        color = vec3(0.8, 1.0, 1.0);
    }

    outColor = vec4(color, 1.0);
}
```

---

## 🌀 4D Visualization Integration

### Tesseract Wireframe Rendering

```cpp
class Tesseract4DVisualizer
{
public:
    Tesseract4DVisualizer()
        : m_tesseract(2.0f)
        , m_rotationXY(0.0f)
        , m_rotationZW(0.0f)
    {
        SetupProjection();
    }

    void Update(const AudioFeatures& audio, float deltaTime)
    {
        // Rotate based on BPM
        float rotationSpeed = audio.bpm / 60.0f * 0.05f;
        m_rotationXY += rotationSpeed;
        m_rotationZW += rotationSpeed * 0.7f;  // Different speed for ZW

        // Scale with energy
        float scale = 0.8f + audio.energy * 0.4f;

        // Apply transformations
        m_tesseract.Reset();
        m_tesseract.Rotate(m_rotationXY, m_rotationZW);
        m_tesseract.SetScale(scale);
    }

    void Render(RenderContext& ctx)
    {
        // Project to 2D
        auto screenPoints = ProjectTesseractTo2D(
            m_tesseract,
            8.0f,  // 4D camera distance
            ctx.projectionMatrix,
            ctx.screenWidth,
            ctx.screenHeight
        );

        // Render edges
        const auto& edges = m_tesseract.GetEdges();

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        std::vector<glm::vec2> lineVertices;

        for (const auto& edge : edges)
        {
            lineVertices.push_back(screenPoints[edge.first]);
            lineVertices.push_back(screenPoints[edge.second]);
        }

        glBufferData(GL_ARRAY_BUFFER,
                     lineVertices.size() * sizeof(glm::vec2),
                     lineVertices.data(), GL_DYNAMIC_DRAW);

        // Draw lines
        m_shader.Bind();
        glDrawArrays(GL_LINES, 0, lineVertices.size());
    }

private:
    Tesseract m_tesseract;
    float m_rotationXY;
    float m_rotationZW;
};
```

---

## 🔮 Quaternion Julia Sets Integration

### Point Cloud Rendering

```cpp
class JuliaSetVisualizer
{
public:
    JuliaSetVisualizer()
    {
        m_julia.SetPreset(JuliaPreset::Spiral);
        RegenerateFractal();
    }

    void Update(const AudioFeatures& audio, float deltaTime)
    {
        // Morph with audio
        m_julia.SetAudioModulation(audio.energy, audio.onsetDetected ? 1.0f : 0.0f, audio.bass);

        // Regenerate periodically (expensive!)
        if (++m_frameCount % 60 == 0)
        {
            RegenerateFractal();
        }
    }

    void Render(RenderContext& ctx)
    {
        const auto& points4D = m_julia.GetPoints();

        // Project to 3D, then to screen
        std::vector<glm::vec2> screenPoints;

        for (const auto& p4d : points4D)
        {
            glm::vec3 p3d = Project4Dto3D(p4d, 5.0f);
            glm::vec2 p2d = Project3Dto2D(p3d, ctx.projectionMatrix,
                                         ctx.screenWidth, ctx.screenHeight);
            screenPoints.push_back(p2d);
        }

        // Upload and render
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     screenPoints.size() * sizeof(glm::vec2),
                     screenPoints.data(), GL_DYNAMIC_DRAW);

        m_shader.Bind();
        glEnable(GL_PROGRAM_POINT_SIZE);
        glDrawArrays(GL_POINTS, 0, screenPoints.size());
    }

private:
    QuaternionJulia m_julia;
    int m_frameCount = 0;

    void RegenerateFractal()
    {
        m_julia.GeneratePointCloud(5000, 2.0f);
    }
};
```

---

## 🎨 Combining Features

### Example: Hybrid Visualization

```cpp
class HybridVisualizer
{
public:
    HybridVisualizer()
        : m_boids(500)
        , m_rd(128, 128)
        , m_tesseract(2.0f)
    {
        // Setup all features
        m_boids.SetSeparationWeight(1.5f);
        m_rd.SetPreset(ReactionDiffusionPreset::Waves);
        m_rd.SeedCenter(10);
    }

    void Update(const AudioFeatures& audio, float deltaTime)
    {
        // Update all systems
        m_boids.SetAudioEnergyMultiplier(1.0f + audio.energy);
        m_boids.Update(deltaTime);

        m_rd.SetAudioModulation(audio.energy, audio.onsetDetected ? 1.0f : 0.0f, 0.5f);
        m_rd.Update(deltaTime);

        m_tesseract.Rotate(audio.beatPhase * 0.1f, audio.beatPhase * 0.07f);
    }

    void Render(RenderContext& ctx)
    {
        // Layer 1: Reaction-Diffusion background
        RenderReactionDiffusion(ctx);

        // Layer 2: 4D Tesseract wireframe (additive blend)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        Render4DTesseract(ctx);

        // Layer 3: Boids particles
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        RenderBoids(ctx);
    }

private:
    BoidsSystem m_boids;
    ReactionDiffusion m_rd;
    Tesseract m_tesseract;
    // ... render methods
};
```

---

## ⚡ Performance Optimization

### GPU Acceleration (Future Work)

```cpp
// Reaction-Diffusion compute shader
class ReactionDiffusionGPU
{
public:
    void Update()
    {
        // Dispatch compute shader
        m_computeShader.Bind();
        glDispatchCompute(m_width / 16, m_height / 16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Swap textures
        std::swap(m_texture1, m_texture2);
    }

private:
    GLuint m_computeShader;
    GLuint m_texture1, m_texture2;
    int m_width, m_height;
};
```

### Spatial Partitioning for Boids

```cpp
// Future optimization: Grid-based spatial hashing
class SpatialGrid
{
public:
    void Insert(VerletParticle* boid)
    {
        int cellX = static_cast<int>(boid->position.x / m_cellSize);
        int cellY = static_cast<int>(boid->position.y / m_cellSize);
        int cellZ = static_cast<int>(boid->position.z / m_cellSize);

        int cellIndex = CellIndex(cellX, cellY, cellZ);
        m_grid[cellIndex].push_back(boid);
    }

    std::vector<VerletParticle*> GetNeighbors(const glm::vec3& position, float radius)
    {
        // Only check relevant cells (27 max)
        std::vector<VerletParticle*> neighbors;
        // ... implementation
        return neighbors;
    }

private:
    float m_cellSize;
    std::unordered_map<int, std::vector<VerletParticle*>> m_grid;
};
```

---

## 📊 Performance Guidelines

| Feature | CPU Cost | GPU Cost | Memory | Optimization Priority |
|---------|----------|----------|---------|----------------------|
| **Boids (1000)** | Medium | Low | Low | High (spatial hashing) |
| **RD (256²)** | High | N/A | Medium | **Critical** (GPU compute) |
| **4D Tesseract** | Very Low | Low | Very Low | Low |
| **Julia (5000)** | Medium | N/A | Medium | Medium (cache results) |

### Recommended Settings

**Desktop (60 FPS):**
- Boids: 1000-2000
- RD: 256×256 (GPU) or 128×128 (CPU)
- Julia: 5000 samples, regen every 2 seconds

**Mobile/Web:**
- Boids: 200-500
- RD: 64×64
- Julia: 1000 samples, regen every 4 seconds

---

## 🎓 Next Steps

1. **Integrate into MilkdropPreset system**
2. **Create custom shape handlers**
3. **GPU compute shaders for RD and Julia**
4. **Spatial optimization for Boids**
5. **Preset authoring tools**

---

**Ready to create the most mind-bending music visualizations ever?** 🚀✨

Go forth and melt faces! 🔥
