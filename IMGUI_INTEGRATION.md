# ImGui Settings UI Integration

This document describes the comprehensive ImGui-based settings UI integration for projectM.

## Overview

A modern, intuitive settings interface has been integrated using Dear ImGui, providing real-time configuration of all projectM settings including core visualization parameters and advanced features (Boids, Reaction-Diffusion, 4D Visualization, Julia Sets).

## Features

### Core Integration
- **Cross-platform OpenGL loader**: Custom loader header that works on macOS, Windows, and Linux
- **Conditional compilation**: ImGui is optional via `ENABLE_SETTINGS_UI` flag
- **Clean separation**: SettingsManager (data) is separate from SettingsUI (presentation)
- **Event handling**: Proper SDL event capture and consumption by ImGui

### Settings Categories

1. **General Settings** (13 parameters)
   - Window configuration (size, fullscreen, aspect correction)
   - Performance (FPS, mesh resolution, texture size)
   - Preset behavior (duration, transitions, hard cuts)

2. **Audio Settings** (11 parameters)
   - Beat detection sensitivity and decay
   - Audio processing (volume boost, filters)
   - Reactivity controls

3. **Boids Flocking** (14 parameters + 4 presets)
   - Population controls
   - Behavior weights (separation, alignment, cohesion)
   - Movement parameters
   - Perception radii
   - Audio reactivity toggles

4. **Reaction-Diffusion** (13 parameters + 5 presets)
   - Grid resolution
   - Gray-Scott parameters (F, k, Du, Dv)
   - Simulation controls
   - Audio-reactive parameters

5. **4D Visualization** (13 parameters + 4 presets)
   - Tesseract properties
   - 4D camera controls
   - Rotation speeds for 4 planes (XY, ZW, XZ, YW)
   - Rendering modes (wireframe, vertices, hybrid, faces)

6. **Julia Set Fractals** (15 parameters + 6 presets)
   - Quaternion c parameter (4D: a, b, c, d components)
   - Quality settings (iterations, sample count)
   - Rendering configuration
   - Animation controls

### UI Features
- Tabbed interface for easy navigation
- Real-time parameter adjustment with instant visual feedback
- Preset selectors for quick configuration
- Tooltips on all parameters
- Save/Load settings to/from files
- Per-category and global reset
- F1 hotkey to toggle UI visibility

## Building

### Prerequisites
- SDL2 development libraries
- OpenGL development libraries
- CMake 3.21+
- C++14 compiler

### CMake Configuration

```bash
# Configure with ImGui support
cmake -B build -DENABLE_SDL_UI=ON

# Build
cmake --build build

# Run
./build/src/sdl-test-ui/projectM-Test-UI
```

### Platform-Specific Notes

**macOS:**
```bash
# Install dependencies
brew install sdl2

# Build
cmake -B build -DENABLE_SDL_UI=ON
cmake --build build
```

**Windows:**
```bash
# Use vcpkg for dependencies
vcpkg install sdl2 opengl

# Build
cmake -B build -DENABLE_SDL_UI=ON -DCMAKE_TOOLCHAIN_FILE=[vcpkg_root]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

**Linux:**
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get install libsdl2-dev libgl1-mesa-dev libglu1-mesa-dev

# Build
cmake -B build -DENABLE_SDL_UI=ON
cmake --build build
```

## Usage

### In-Application Controls
- **F1**: Toggle settings UI
- **ESC**: Close settings UI (in UI window)
- **Mouse**: Interact with UI controls

### File Operations
1. Click "File" → "Save Settings..." to save current configuration
2. Click "File" → "Load Settings..." to load saved configuration
3. Click "File" → "Reset All to Defaults" to restore defaults

### Applying Presets
1. Navigate to the desired category tab (e.g., "Boids Flocking")
2. Use the preset dropdown at the top
3. Select a preset to instantly apply its parameters
4. Modify individual parameters to create custom configurations

## Architecture

### File Structure

```
src/libprojectM/Settings/
├── SettingsManager.hpp      # Settings data structures and persistence
├── SettingsManager.cpp      # Implementation
├── SettingsUI.hpp            # ImGui UI interface
├── SettingsUI.cpp            # UI implementation (850+ lines)
└── CMakeLists.txt            # Build configuration

vendor/imgui/
├── CMakeLists.txt            # Cross-platform CMake with custom GL loader
└── ...                       # ImGui source (from chrystaleat/imgui fork)

src/sdl-test-ui/
├── pmSDL.hpp                 # Updated with SettingsManager/UI members
├── pmSDL.cpp                 # Updated with init, event handling, rendering
└── CMakeLists.txt            # Updated to link ImGui
```

### Integration Points

**Initialization** (pmSDL.cpp:448)
```cpp
#ifdef ENABLE_SETTINGS_UI
    _settingsManager = std::make_unique<SettingsManager>();
    _settingsUI = std::make_unique<SettingsUI>(_projectM, *_settingsManager);
    _settingsUI->Init(window, _openGlContext);
#endif
```

**Event Handling** (pmSDL.cpp:337)
```cpp
#ifdef ENABLE_SETTINGS_UI
    if (_settingsUI && _settingsUI->ProcessEvent(&evt))
        continue; // Event consumed by UI
#endif
```

**Rendering** (pmSDL.cpp:464)
```cpp
#ifdef ENABLE_SETTINGS_UI
    if (_settingsUI) {
        _settingsUI->NewFrame();
        _settingsUI->Render();
        _settingsUI->EndFrame();
    }
#endif
```

**Cleanup** (pmSDL.cpp:48)
```cpp
#ifdef ENABLE_SETTINGS_UI
    if (_settingsUI) {
        _settingsUI->Shutdown();
        _settingsUI.reset();
    }
    _settingsManager.reset();
#endif
```

### Custom OpenGL Loader

The `vendor/imgui/CMakeLists.txt` generates a custom OpenGL loader header at build time:

```cpp
// Generated: imgui_impl_opengl3_loader.h
#pragma once
#ifdef __APPLE__
# include <OpenGL/gl3.h>
# include <OpenGL/gl3ext.h>
#elif defined(_WIN32)
# include <GL/glew.h>
#else
# include <GL/gl.h>
# include <GL/glext.h>
#endif
```

This ensures ImGui uses the correct OpenGL headers for each platform, matching projectM's setup.

## Settings File Format

Settings are saved in INI format:

```ini
# Boids Flocking Settings
Boids.MaxBoids = 100
Boids.SpawnCount = 50
Boids.SeparationWeight = 1.500000
Boids.AlignmentWeight = 1.000000
Boids.CohesionWeight = 1.000000
...

# Reaction-Diffusion Settings
RD.GridWidth = 256
RD.GridHeight = 256
RD.FeedRate = 0.055000
RD.KillRate = 0.062000
...
```

## Extending the UI

### Adding a New Setting

1. **Add to Settings Structure** (SettingsManager.hpp)
```cpp
struct MyFeatureSettings {
    float parameter1 = 1.0f;
    int parameter2 = 10;
    bool enabled = true;
};
```

2. **Add Persistence** (SettingsManager.cpp)
```cpp
// In SaveToFile:
file << "MyFeature.Parameter1 = " << settings.parameter1 << "\n";

// In LoadFromFile:
else if (key == "MyFeature.Parameter1")
    settings.parameter1 = std::stof(value);
```

3. **Add UI Panel** (SettingsUI.cpp)
```cpp
void SettingsUI::RenderMyFeatureSettings() {
    auto& settings = m_settingsManager.GetMyFeatureSettings();
    ImGui::TextWrapped("My feature description");
    RenderFloatSlider("Parameter 1", &settings.parameter1, 0.0f, 10.0f);
    RenderIntSlider("Parameter 2", &settings.parameter2, 1, 100);
    RenderBoolSetting("Enabled", &settings.enabled);
}
```

4. **Add Tab** (SettingsUI.cpp Render method)
```cpp
if (ImGui::BeginTabItem("My Feature")) {
    RenderMyFeatureSettings();
    ImGui::EndTabItem();
}
```

## Known Limitations

- Settings UI is only available when SDL UI is enabled
- Requires OpenGL 3.x+ (set via GLSL version in SettingsUI.cpp:105-111)
- Some projectM core settings require application restart to take effect
- File dialogs are basic text input (could be enhanced with native dialogs)

## Future Enhancements

- Native file picker dialogs (via ImGuiFileDialog or platform APIs)
- Keyboard shortcuts for common actions
- Settings search/filter
- Undo/redo for parameter changes
- Parameter value history/animation
- Export settings as JSON instead of INI
- Network/OSC control integration
- MIDI mapping for live performance

## Troubleshooting

### ImGui doesn't build
- Ensure the imgui submodule is initialized: `git submodule update --init vendor/imgui`
- Check that SDL2 is installed: `sdl2-config --version`
- Verify OpenGL headers are present

### UI doesn't appear
- Press F1 to toggle visibility
- Check console for "Settings UI initialized successfully" message
- Verify `ENABLE_SETTINGS_UI` was defined at compile time

### UI is laggy
- Reduce mesh resolution in General settings
- Lower FPS target
- Disable VSync if using external FPS limiter

### Settings don't persist
- Check file save location (provide full path)
- Ensure write permissions in save directory
- Verify settings file is valid INI format

## Credits

- **ImGui**: Omar Cornut (ocornut) - https://github.com/ocornut/imgui
- **ImGui Fork**: chrystaleat/imgui (custom OpenGL loader integration)
- **projectM**: projectM Team - https://github.com/projectM-visualizer
- **Integration**: Claude Code - Cross-platform ImGui integration

## License

The ImGui integration follows projectM's LGPL 2.1 license. ImGui itself is licensed under MIT.
