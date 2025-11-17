# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**projectM** is a high-performance, cross-platform music visualization library that reimplements Winamp Milkdrop. It's a C++ library (v4.1.0) that transforms audio input into mesmerizing real-time visual effects using OpenGL rendering.

Key facts:
- Library-only design: Clients provide audio, projectM renders visuals
- C API for maximum compatibility, optional C++ interface
- Cross-platform: Windows, Linux, macOS, Android, iOS, WebGL/WASM
- Active development with recent advanced features (4D visualization, fractals, physics simulation)

## Common Development Commands

### Building the Project

```bash
# Standard build (from project root)
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_SDL_UI=ON ..
cmake --build . -- -j$(nproc)

# Debug build with tests
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DENABLE_SDL_UI=ON ..
cmake --build . -- -j$(nproc)

# Build specific targets
cmake --build . --target projectM-4          # Main library only
cmake --build . --target projectMSDL         # SDL test application
cmake --build . --target projectM-4-playlist # Playlist library
```

### Running Tests

```bash
# Run all tests (from build directory)
ctest --output-on-failure

# Run specific test
ctest -R AudioFeatureExtractor --output-on-failure

# Run tests with verbose output
ctest -V

# Run a single test binary directly
./tests/projectM-unittest-AudioFeatureExtractor
```

### Running the SDL Test Application

```bash
# From build directory
./src/sdl-test-ui/projectMSDL

# With specific preset directory
./src/sdl-test-ui/projectMSDL --presetPath ../presets
```

### Building Demos

```bash
# Build all demos
cmake --build . --target BoidsDemo
cmake --build . --target ReactionDiffusionDemo
cmake --build . --target Hyperdimensional4DDemo
cmake --build . --target QuaternionJuliaDemo

# Run a demo
./demos/BoidsDemo
```

## Code Architecture

### Core Components

**Audio System** (`src/libprojectM/Audio/`)
- `PCM`: Raw audio input handling (float32, int16, uint8)
- `MilkdropFFT`: FFT analysis (512 frequency bands)
- `AudioFeatureExtractor`: Beat detection, energy analysis
- `WaveformAligner`: Temporal smoothing
- Thread-safe PCM queue with atomic synchronization

**Rendering System** (`src/libprojectM/Renderer/`)
- `Renderer`: Main rendering orchestration
- `Framebuffer`: Offscreen rendering targets
- `ShaderCache`: GLSL shader management
- `TextureManager`: GPU texture handling
- Advanced effects: Boids, ReactionDiffusion, Tesseract, QuaternionJulia, VerletParticle

**Milkdrop Presets** (`src/libprojectM/MilkdropPreset/`)
- `MilkdropPreset`: Main preset implementation
- `MilkdropShader`: HLSL-to-GLSL translation
- Expression evaluation via projectm-eval (bison/flex parser)
- Custom shapes and waveforms

**Public API** (`src/api/include/projectM-4/`)
- C interface for broad compatibility
- Opaque pointer pattern for ABI stability
- Headers: core.h, audio.h, render_opengl.h, callbacks.h

### Key Design Patterns

1. **Library-first**: projectM is purely a library; clients handle audio capture and windowing
2. **C API priority**: Public interface is C for maximum compatibility
3. **OpenGL abstraction**: Supports both Core Profile (3.3+) and ES (3.2)
4. **Modular subsystems**: Audio, Rendering, Presets are loosely coupled
5. **Header-only math**: GLM bundled for dependency simplification

## Important Implementation Notes

### Audio Processing
- PCM data must be provided continuously for smooth visualization
- Supports mono/stereo, various sample formats
- Beat detection uses onset detection algorithm
- FFT provides 512 frequency bands for spectrum analysis

### Rendering Pipeline
1. Audio analysis generates per-frame features
2. Preset equations evaluate using audio features
3. GLSL shaders render visualization passes
4. Multiple framebuffers enable feedback effects
5. Final output to screen or texture

### Memory Management
- RAII with smart pointers throughout
- Weak pointers prevent circular dependencies
- Thread-safe audio queue with lock-free operations where possible

### Security Considerations
- Extensive null pointer checks
- Buffer overflow protections
- Integer overflow validation
- Symlink TOCTOU mitigation in file operations
- See SECURITY_AUDIT_REPORT.md for details

## Build Options

Key CMake options:
- `ENABLE_SDL_UI`: Build SDL2 test application (default: OFF)
- `BUILD_TESTING`: Enable test suite (default: OFF)
- `BUILD_SHARED_LIBS`: Build shared vs static libraries
- `ENABLE_GLES`: Use OpenGL ES instead of desktop OpenGL
- `ENABLE_EMSCRIPTEN`: Build for WebGL/WASM
- `ENABLE_PLAYLIST`: Build playlist management library
- `ENABLE_SYSTEM_GLM`: Use system GLM instead of bundled

## Testing Strategy

Tests use Google Test framework and cover:
- Audio feature extraction accuracy
- Physics simulation (Verlet, Boids)
- Mathematical algorithms (4D projections, quaternions)
- Pattern generation (reaction-diffusion)
- Security validations (buffer bounds, null checks)
- Preset parsing and evaluation

## Recent Advanced Features

The project recently added cutting-edge visualization capabilities:

1. **Boids Flocking** (`Renderer/BoidsSystem`): Craig Reynolds' algorithm with audio reactivity
2. **Reaction-Diffusion** (`Renderer/ReactionDiffusion`): Gray-Scott chemical patterns
3. **4D Visualization** (`Renderer/Tesseract`): Hyperdimensional projections
4. **Quaternion Fractals** (`Renderer/QuaternionJulia`): 4D Julia sets
5. **Verlet Physics** (`Renderer/VerletParticle`): Constraint-based particle simulation

Each feature has corresponding tests and demo applications in the demos/ directory.

## Platform-Specific Notes

### Windows
- Requires GLEW for OpenGL extensions
- Use vcpkg for dependency management
- MSVC compiler recommended

### macOS
- Supports both x86_64 and Apple Silicon
- May require code signing for distribution
- Use Homebrew or vcpkg for dependencies

### Linux
- Requires OpenGL development headers
- SDL2 for test application
- Package managers typically have projectM packages

### Android/iOS
- Use OpenGL ES 3.2
- Special build flags required (ENABLE_GLES)
- See platform-specific build documentation

## Code Style

- C++14 minimum standard
- Lines must not exceed 100 characters (per global CLAUDE.md)
- PEP 8 styling for any Python files
- Use clang-format for automatic formatting
- Follow existing code patterns in each module