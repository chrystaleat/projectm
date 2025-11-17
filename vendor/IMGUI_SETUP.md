# ImGui Setup Instructions

ProjectM now includes a beautiful ImGui-based settings interface! 🎨

## Quick Setup

### Option 1: Git Submodule (Recommended)

```bash
cd /home/user/projectm/vendor
git submodule add https://github.com/ocornut/imgui.git imgui
git submodule update --init --recursive
```

### Option 2: Manual Download

```bash
cd /home/user/projectm/vendor
git clone https://github.com/ocornut/imgui.git imgui
```

### Required ImGui Files

The following files from ImGui are needed:

**Core:**
- imgui.h
- imgui.cpp
- imgui_demo.cpp
- imgui_draw.cpp
- imgui_tables.cpp
- imgui_widgets.cpp
- imconfig.h
- imgui_internal.h
- imstb_rectpack.h
- imstb_textedit.h
- imstb_truetype.h

**Backends (for SDL + OpenGL):**
- backends/imgui_impl_sdl2.h
- backends/imgui_impl_sdl2.cpp
- backends/imgui_impl_opengl3.h
- backends/imgui_impl_opengl3.cpp

## Version Tested

ImGui v1.90.0+ (docking branch recommended for advanced layouts)

## Integration

Once ImGui is in `vendor/imgui/`, the CMake build will automatically:
1. Find and compile ImGui sources
2. Link with projectM
3. Enable the settings UI in SDL test application

## Usage

Launch projectM SDL app and press **F1** to open settings!

```bash
./projectm-sdl
# Press F1 for settings UI
# Press ESC to close settings
```

## Features

The ImGui settings UI includes:
- 📊 **6 Settings Categories** (General, Audio, Boids, RD, 4D, Julia)
- 🎨 **Preset Browsers** with one-click application
- 👁️ **Live Preview** of visual changes
- 💾 **Save/Load** settings to JSON
- 🔄 **Reset** to defaults (per-category or all)
- ⌨️ **Hotkeys** for quick access
- 📈 **Visual Feedback** (sliders, checkboxes, color pickers)
- 🎯 **Organized Tabs** for easy navigation

## Customization

Edit `src/libprojectM/Settings/SettingsUI.cpp` to customize:
- UI layout and style
- Widget types and ranges
- Color schemes
- Hotkeys

## Troubleshooting

**ImGui not found:**
```
CMake Error: Could not find imgui
```
→ Run the setup commands above

**Linking errors:**
→ Make sure SDL2 development libraries are installed

**UI not appearing:**
→ Press F1 to toggle settings UI
→ Check that ENABLE_IMGUI=ON in CMake

---

**Enjoy the beautiful settings interface!** ✨
