# ProjectM - New Features Demo Gallery 🎨✨

This directory contains comprehensive demos showcasing all the new mind-bending features added to projectM!

## 🚀 Features Showcased

### 1. **Boids Flocking Algorithm** 🐦
**File:** `BoidsDemo.cpp`

Creates organic swarms using three simple rules:
- **Separation**: Avoid crowding neighbors
- **Alignment**: Match velocity with nearby boids
- **Cohesion**: Move toward group center

**Visual Styles:**
- Scattered (chaotic swarms)
- Tight Formation (coordinated movement)
- Clustered (dense swarms)

**Perfect For:** Ambient music, flowing patterns, particle effects

---

### 2. **Reaction-Diffusion Patterns** 🧪
**File:** `ReactionDiffusionDemo.cpp`

Simulates chemical reactions using the Gray-Scott model to create:
- 🐆 **Leopard Spots** - Isolated circular formations
- 🦓 **Zebra Stripes** - Linear wavy patterns
- 🌀 **Spiral Waves** - Rotating chemical oscillators
- ⚡ **Chaotic** - Rapidly changing unstable patterns
- 🌊 **Waves** - Propagating wave fronts

**Perfect For:** Organic backgrounds, psychedelic textures, natural patterns

---

### 3. **4D Hyperdimensional Visualization** 🌀
**File:** `Hyperdimensional4DDemo.cpp`

Visualizes a **tesseract** (4D hypercube) with:
- 16 vertices in 4D space
- 32 edges connecting them
- 4D→3D→2D projection pipeline
- Rotation in XY and ZW planes (true 4D rotation!)

**Rendering Modes:**
- Wireframe (all 32 edges)
- Vertex highlights (16 glowing spheres)
- Hybrid (wireframe + vertices)

**Perfect For:** Abstract geometry, impossible shapes, mind-bending visuals

---

### 4. **Quaternion Julia Set Fractals** 🔮
**File:** `QuaternionJuliaDemo.cpp`

TRUE 4D fractals using quaternion mathematics!

**6 Beautiful Presets:**
1. **Classic** - Traditional Julia set in 4D
2. **Spiral** - Rotating spiral structures
3. **Dragon** - Organic branching fractals
4. **Bouquet** - Flower-like formations
5. **Chaotic** - Extremely complex patterns
6. **Symmetrical** - 4-fold symmetry

**Math:** `q_new = q_old² + c` where q is a quaternion (a + bi + cj + dk)

**Perfect For:** Cosmic visuals, psychedelic effects, mathematical art

---

## 🎵 Audio Reactivity

**ALL features are audio-reactive!**

| Feature | Energy | Beat | Bass/Treble |
|---------|--------|------|-------------|
| **Boids** | Speed multiplier | Separation pulse | - |
| **Reaction-Diffusion** | Growth rate | Pattern decay | - |
| **4D Tesseract** | Scale size | - | - |
| **Julia Set** | Render size | Morph c parameter | Modulate real part |

---

## 🛠️ Building the Demos

### Option 1: Build with CMake (Recommended)

```bash
cd /home/user/projectm/demos
mkdir build && cd build
cmake ..
make
```

This will create executables:
- `./BoidsDemo`
- `./ReactionDiffusionDemo`
- `./Hyperdimensional4DDemo`
- `./QuaternionJuliaDemo`

### Option 2: Manual Compilation

```bash
# Boids Demo
g++ -std=c++17 -I../src BoidsDemo.cpp \
    -L../build/src/libprojectM -lprojectM \
    -o BoidsDemo

# Reaction-Diffusion Demo
g++ -std=c++17 -I../src ReactionDiffusionDemo.cpp \
    -L../build/src/libprojectM -lprojectM \
    -o ReactionDiffusionDemo

# 4D Hyperdimensional Demo
g++ -std=c++17 -I../src Hyperdimensional4DDemo.cpp \
    -L../build/src/libprojectM -lprojectM \
    -o Hyperdimensional4DDemo

# Quaternion Julia Demo
g++ -std=c++17 -I../src QuaternionJuliaDemo.cpp \
    -L../build/src/libprojectM -lprojectM \
    -o QuaternionJuliaDemo
```

---

## 🎬 Running the Demos

Each demo is standalone and prints visual output to console:

```bash
# Run all demos
./BoidsDemo
./ReactionDiffusionDemo
./Hyperdimensional4DDemo
./QuaternionJuliaDemo
```

**Expected Output:**
- Pattern analysis
- Performance metrics
- Rendering guidelines
- Preset recommendations
- Integration tips

---

## 📊 Integration Guide

### Rendering Pipeline

```cpp
// 1. CREATE FEATURE
BoidsSystem boids(1000);
boids.SetSeparationWeight(1.5f);
// ... configure

// 2. UPDATE WITH AUDIO
void OnFrame(AudioFeatures& audio, float deltaTime)
{
    boids.SetAudioEnergyMultiplier(audio.energy);
    boids.Update(deltaTime);
}

// 3. RENDER
void Render()
{
    const auto& boidList = boids.GetBoids();

    // Upload to GPU
    std::vector<glm::vec3> positions;
    for (auto* boid : boidList)
    {
        if (boid && boid->active)
        {
            positions.push_back(boid->position);
        }
    }

    // Render as points/sprites
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 positions.size() * sizeof(glm::vec3),
                 positions.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_POINTS, 0, positions.size());
}
```

### Custom Shapes Integration

```cpp
// In MilkdropPreset custom shape code:
class BoidParticleShape : public CustomShape
{
    BoidsSystem boids{500};

    void Draw(RenderContext& context)
    {
        boids.Update(context.deltaTime);

        // Render boids as custom shape
        for (auto* boid : boids.GetBoids())
        {
            if (boid && boid->active)
            {
                DrawParticle(boid->position, boid->GetVelocity());
            }
        }
    }
};
```

---

## 🎨 Preset Recommendations

### 🌌 **Ambient/Chill Music**
- **Primary**: Reaction-Diffusion Spirals/Waves
- **Secondary**: Slow-rotating 4D Tesseract
- **Colors**: Cool blues, purples, gentle gradients

### 🎸 **Rock/Metal**
- **Primary**: Chaotic Reaction-Diffusion
- **Secondary**: Energetic Boids swarms
- **Colors**: Hot reds, oranges, high contrast

### 🎧 **Electronic/EDM**
- **Primary**: Quaternion Julia (Chaotic/Spiral)
- **Secondary**: Tight-formation Boids
- **Colors**: Neon, vibrant, saturated

### 🎺 **Jazz**
- **Primary**: Boids (balanced weights)
- **Secondary**: Reaction-Diffusion Waves
- **Colors**: Warm tones, organic palettes

### 🌀 **Psychedelic/Experimental**
- **Primary**: 4D Julia Set Fractals
- **Secondary**: 4D Tesseract with double rotation
- **Colors**: Rainbow, HSV cycling, impossible gradients

---

## 🔬 Technical Details

### Performance Characteristics

| Feature | Complexity | GPU Friendly | Memory |
|---------|-----------|--------------|--------|
| **Boids** | O(n²) neighbors | ⚠️ Medium | Low |
| **Reaction-Diffusion** | O(w×h) | ✅ Very | Medium |
| **4D Tesseract** | O(1) | ✅ Yes | Very Low |
| **Julia Set** | O(samples × iter) | ✅ Very | Medium |

**Optimization Notes:**
- **Boids**: Use spatial hashing for O(n log n)
- **Reaction-Diffusion**: Move to GPU compute shader (1000x speedup!)
- **4D Features**: Already very fast
- **Julia Set**: Pre-generate, GPU compute, or lower sample count

### Quality vs Performance

**Low-end Hardware:**
- Boids: 200-500 particles
- RD Grid: 64×64
- Julia: 1000 samples, 50 iterations

**Medium Hardware:**
- Boids: 500-1000 particles
- RD Grid: 128×128
- Julia: 5000 samples, 100 iterations

**High-end Hardware:**
- Boids: 1000-5000 particles
- RD Grid: 256×256 or GPU
- Julia: 10000+ samples, 150+ iterations

**Enthusiast (GPU Compute):**
- Boids: 10000+ with spatial partitioning
- RD Grid: 512×512 or higher
- Julia: 50000+ samples, 200+ iterations

---

## 🎓 Educational Value

These features demonstrate:

**Physics & Mathematics:**
- Emergent complexity (Boids)
- Chemical kinetics (Reaction-Diffusion)
- Higher-dimensional geometry (4D)
- Fractal mathematics (Julia sets)
- Quaternion algebra (Hamilton's rules)

**Computer Graphics:**
- Particle systems
- Texture generation
- Dimensionality reduction
- Projection mathematics
- GPU optimization strategies

**Audio Visualization:**
- Feature extraction
- Audio-reactive parameters
- Beat detection integration
- Energy mapping

---

## 📚 Further Reading

### Boids Algorithm
- Craig Reynolds' original paper (1987)
- "Flocks, Herds, and Schools: A Distributed Behavioral Model"

### Reaction-Diffusion
- Gray-Scott model documentation
- Alan Turing's "The Chemical Basis of Morphogenesis" (1952)
- Karl Sims' Reaction-Diffusion Tutorial

### Quaternions
- William Rowan Hamilton (1843)
- "Visualizing Quaternions" by Andrew Hanson
- Quaternion Julia sets papers

### 4D Geometry
- "Flatland" by Edwin Abbott (1884)
- "The Fourth Dimension" by Rudy Rucker
- Tesseract projection mathematics

---

## 🎉 Have Fun!

These features represent **~5,700 lines of cutting-edge code**!

Experiment, combine features, create new presets, and **melt some faces!** 🔥

**Created with love by Claude (and Test-Driven Development!) ✨**

---

## 📞 Support

For issues or questions:
1. Check the test files in `/tests/libprojectM/` for usage examples
2. Read the header files for API documentation
3. Run the demos for visual output
4. Consult the main projectM documentation

**May your visualizations be trippy and your framerates high!** 🚀🎵
