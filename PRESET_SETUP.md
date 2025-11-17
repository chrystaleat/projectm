# ProjectM Preset Setup Guide

## Issue: Only One Preset Showing ("M with Headphones")

If you're seeing only one preset (the default "M with headphones" visualization) and can't switch to other presets, this is because **the presets directory is empty** or not configured correctly.

## Understanding the Problem

projectM requires **preset files** (`.milk` files) to create visualizations. When the application can't find presets:
- It falls back to a built-in default preset
- Preset switching keys don't work because there's nothing to switch to

## Keyboard Controls for Preset Switching

Once you have presets installed, use these keys:

- **Left Arrow** → Previous preset
- **Right Arrow** → Next preset
- **Mouse Wheel Up** → Previous preset
- **Mouse Wheel Down** → Next preset
- **R** → Random preset
- **Y** → Toggle shuffle mode
- **Space** → Lock/unlock current preset
- **F1** → Toggle Settings UI (if ImGui is enabled)

## Solution: Get Presets!

### Option 1: Download Community Presets (Recommended)

The best presets come from the Milkdrop community:

**Official projectM Presets Repository:**
```bash
# Clone the presets repository
git clone https://github.com/projectM-visualizer/projectm-presets.git

# Copy presets to your projectM directory
cp -r projectm-presets/presets/* /path/to/projectM/presets/
```

**Popular Preset Packs:**
1. [Milkdrop 2 Presets](https://github.com/projectM-visualizer/presetm-presets) - Official collection
2. [Community Presets](https://github.com/projectM-visualizer/community-presets) - User contributions

### Option 2: Use Test Presets (For Development)

For quick testing, use the included test presets:

```bash
# From projectM root directory
cp presets/tests/*.milk presets/

# Or update config.inp to point to tests directory
# Edit src/sdl-test-ui/config.inp:
# Preset Path = presets/tests
```

### Option 3: Download Classic Winamp/Milkdrop Presets

projectM is compatible with classic Winamp Milkdrop presets:

1. Search for "Milkdrop presets" online
2. Download `.milk` files
3. Copy to `presets/` directory
4. Restart projectM

## Preset Directory Locations

projectM looks for presets in different locations depending on how it's built:

### Debug Build:
```
./presets/          # Current working directory
```

### Release Build:
```
/usr/local/share/projectM/presets/      # macOS/Linux
C:\Program Files\projectM\presets\      # Windows
```

### Custom Location:
Edit `src/sdl-test-ui/config.inp`:
```ini
Preset Path = /your/custom/path/to/presets
```

## Verifying Preset Installation

After adding presets:

1. **Check the directory:**
   ```bash
   ls presets/*.milk | wc -l
   # Should show number of preset files
   ```

2. **Run projectM:**
   ```bash
   ./build/src/sdl-test-ui/projectM-Test-UI
   ```

3. **Look for console output:**
   ```
   Loading presets from: /path/to/presets
   Loaded N presets
   ```

4. **Try switching presets:**
   - Press **Right Arrow** key
   - Should see a different visualization
   - Window title should update with preset name

## Creating Your Own Presets

Presets are text files with `.milk` extension containing equations and parameters.

### Minimal Preset Example:

```ini
[preset00]
fRating=3.000000
fGammaAdj=1.000000
fDecay=0.980000
fVideoEchoZoom=1.000000
fVideoEchoAlpha=0.000000
nVideoEchoOrientation=0
nWaveMode=0
bAdditiveWaves=0
bWaveDots=0
bWaveThick=0
bModWaveAlphaByVolume=0
bMaximizeWaveColor=1
bTexWrap=1
bDarkenCenter=0
bRedBlueStereo=0
bBrighten=0
bDarken=0
bSolarize=0
bInvert=0
fWaveAlpha=0.800000
fWaveScale=1.000000
fWaveSmoothing=0.750000
fWaveParam=0.000000
fModWaveAlphaStart=0.750000
fModWaveAlphaEnd=0.950000
fWarpAnimSpeed=1.000000
fWarpScale=1.000000
fZoomExponent=1.000000
fShader=0.000000
zoom=1.000000
rot=0.000000
cx=0.500000
cy=0.500000
dx=0.000000
dy=0.000000
warp=1.000000
sx=1.000000
sy=1.000000
wave_r=0.500000
wave_g=0.500000
wave_b=0.500000
wave_x=0.500000
wave_y=0.500000
ob_size=0.010000
ob_r=0.000000
ob_g=0.000000
ob_b=0.000000
ob_a=0.000000
ib_size=0.010000
ib_r=0.250000
ib_g=0.250000
ib_b=0.250000
ib_a=0.000000
nMotionVectorsX=12.000000
nMotionVectorsY=9.000000
mv_dx=0.000000
mv_dy=0.000000
mv_l=0.900000
mv_r=1.000000
mv_g=1.000000
mv_b=1.000000
mv_a=0.000000
```

Save as `my_preset.milk` in the presets directory!

## Troubleshooting

### "No presets found" error:
- Check that `.milk` files exist in the presets directory
- Verify the `Preset Path` in config.inp is correct
- Check file permissions (must be readable)

### Presets load but look broken:
- Some old presets may not be fully compatible
- Try different presets from the official repository
- Check console for error messages

### Can't switch presets:
- Verify you have more than one `.milk` file
- Try disabling preset lock (press **Space** key)
- Check that keyboard is not captured by ImGui UI (press **F1** to close it)

### Window title doesn't update:
- Normal - title updates on preset switch
- If stuck, try pressing **R** for random preset

## Recommended Starter Presets

From the official repository, these are excellent starting points:

1. **Geiss - Spiral Artifact.milk** - Classic spiral visualization
2. **Rovastar - Altars of Madness.milk** - Complex geometric patterns
3. **Martin - Xion.milk** - Smooth flowing shapes
4. **Unchained - Witchcraft Evolution.milk** - Dynamic color morphing
5. **Flexi - Truly Soft Piece of Heaven.milk** - Calm, atmospheric

## Learning More

- **Milkdrop Documentation**: https://www.geisswerks.com/milkdrop/
- **Preset Development Guide**: https://github.com/projectM-visualizer/projectm/wiki/Presets
- **Community Forum**: https://github.com/projectM-visualizer/projectm/discussions

## Code Fix for Preset Path Reading

If you're developing projectM and want to read the `Preset Path` from config.inp,
modify `src/sdl-test-ui/setup.cpp`:

```cpp
// Around line 196, replace:
std::string presetURL = base_path + "/presets";

// With:
std::string presetURL;
try {
    ConfigFile config(configFilePath);
    presetURL = config.read<std::string>("Preset Path", base_path + "/presets");
    // Make path absolute if relative
    if (presetURL[0] != '/' && presetURL[1] != ':') {  // Not absolute
        presetURL = base_path + "/" + presetURL;
    }
} catch (...) {
    presetURL = base_path + "/presets";  // Fallback
}
```

This allows users to configure the preset path in config.inp!
