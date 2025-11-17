# projectM Visualizer - User Guide

## Quick Start (3 Ways to Run)

### 1. Double-Click Launch (Easiest)
Just double-click `ProjectM.command` in Finder

### 2. Terminal Launcher Script
```bash
./projectM-launcher.sh          # Run with default presets
./projectM-launcher.sh run-cotc  # Run with 10K+ presets (after installing)
./projectM-launcher.sh help      # Show all options
```

### 3. Direct Execution
```bash
./build/src/sdl-test-ui/projectM-Test-UI --presetPath presets/tests
```

## Installing Preset Packs

Run the installer to download amazing preset collections:
```bash
./projectM-launcher.sh install
```

Recommended: **Cream of the Crop** (10,000+ curated presets)

## Configuration Settings

Edit your settings: `~/.projectM/config.inp`

Or use the launcher:
```bash
./projectM-launcher.sh config
```

### Key Settings:
- **FPS**: 60 (increase for smoother visuals on M4 Pro)
- **Preset Duration**: 10 seconds (how long each preset plays)
- **Smooth Transition Duration**: 1 second (blend time between presets)
- **Beat Sensitivity**: 1.0 (0-5, higher = more reactive)
- **Hard Cuts Enabled**: false (enable for beat-synced transitions)
- **Window Width/Height**: 512 (initial size, resizable)
- **Mesh X/Y**: 708x400 (resolution of effects, increase for quality)

## Keyboard Controls

### Navigation
- **SPACE** - Lock/unlock current preset
- **← / →** - Previous/Next preset
- **↑ / ↓** - Increase/Decrease beat sensitivity
- **R** - Random preset
- **Y** - Toggle shuffle mode

### Display
- **⌘+F** - Fullscreen
- **⌘+M** - Switch monitor
- **⌘+S** - Stretch across multiple monitors
- **A** - Toggle aspect correction

### System
- **⌘+I** - Toggle audio input device
- **⌘+Q** - Quit

## Audio Setup

### Option 1: Microphone Input (Default)
The app captures audio from your default microphone - plays music nearby and it will react!

### Option 2: System Audio (Recommended)
Install **BlackHole** (free) to route system audio:

1. Download: https://existential.audio/blackhole/
2. Install BlackHole 2ch
3. Create Multi-Output Device:
   - Open Audio MIDI Setup
   - Click "+" → Create Multi-Output Device
   - Check both "BlackHole 2ch" and your speakers
   - Set as default output

### Option 3: Music Apps
Some music apps (like iTunes visualizer mode) can send audio directly.

## Settings Interface

**There is NO GUI for settings** - projectM uses a text config file by design. This keeps it lightweight and focused on the visuals.

Edit settings:
1. Open Terminal and run: `./projectM-launcher.sh config`
2. Or manually edit: `~/.projectM/config.inp`
3. Changes take effect on next launch

## Performance Tips for M4 Pro

Your M4 Pro can handle maximum quality settings:

```ini
# ~/.projectM/config.inp optimized for M4 Pro
Mesh X = 1920      # Max quality mesh
Mesh Y = 1080
FPS = 120          # Smooth 120fps if your display supports it
```

## Troubleshooting

### No Audio Response?
1. Check audio input: Press **⌘+I** to cycle inputs
2. Increase volume of audio source
3. Increase Beat Sensitivity: Press **↑** key

### Presets Not Loading?
Check preset path in config file or use launcher:
```bash
./projectM-launcher.sh run-custom
# Then enter path to your preset folder
```

### Want More Presets?
```bash
./projectM-launcher.sh install
# Choose option 1 for 10,000+ presets
```

## File Locations

- **Config**: `~/.projectM/config.inp`
- **Application**: `/Users/chrysteat/siren/projectm/build/src/sdl-test-ui/projectM-Test-UI`
- **Launcher**: `/Users/chrysteat/siren/projectm/projectM-launcher.sh`
- **Presets**: `/Users/chrysteat/siren/projectm/presets-*`

## Advanced Features

The M4 Pro can handle these experimental features:
- 4D Hyperdimensional visualization
- Quaternion Julia fractals
- Reaction-diffusion patterns
- Boids flocking algorithms
- Verlet physics simulation

These are integrated into newer presets automatically.

---

Enjoy the psychedelic journey! 🎵✨