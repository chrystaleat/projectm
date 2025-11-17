#!/bin/bash
# projectM Visualizer Launcher Script
# Easy-to-use launcher with preset management

# Colors for terminal output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Set paths
PROJECT_DIR="/Users/chrysteat/siren/projectm"
BUILD_DIR="$PROJECT_DIR/build"
EXECUTABLE="$BUILD_DIR/src/sdl-test-ui/projectM-Test-UI"
CONFIG_FILE="$HOME/.projectM/config.inp"

# Function to print colored messages
print_msg() {
    echo -e "${2}$1${NC}"
}

# Function to show help
show_help() {
    print_msg "projectM Visualizer Launcher" "$BLUE"
    echo ""
    print_msg "Usage: $0 [OPTIONS]" "$GREEN"
    echo ""
    echo "OPTIONS:"
    echo "  run         - Run with default presets"
    echo "  run-cotc    - Run with Cream of the Crop presets (10K+)"
    echo "  run-custom  - Run with custom preset directory"
    echo "  install     - Install preset packs"
    echo "  config      - Edit configuration file"
    echo "  rebuild     - Rebuild projectM from source"
    echo "  help        - Show this help message"
    echo ""
    print_msg "Keyboard Controls:" "$YELLOW"
    echo "  SPACE       - Lock/unlock current preset"
    echo "  ←/→         - Previous/Next preset"
    echo "  ↑/↓         - Increase/Decrease beat sensitivity"
    echo "  A           - Toggle aspect correction"
    echo "  R           - Random preset"
    echo "  Y           - Toggle shuffle mode"
    echo "  CMD+F       - Toggle fullscreen"
    echo "  CMD+M       - Change monitor"
    echo "  CMD+S       - Stretch across monitors"
    echo "  CMD+I       - Toggle audio input"
    echo "  CMD+Q       - Quit"
}

# Function to check if projectM is built
check_build() {
    if [ ! -f "$EXECUTABLE" ]; then
        print_msg "Error: projectM not built. Run '$0 rebuild' first." "$RED"
        exit 1
    fi
}

# Function to install preset packs
install_presets() {
    print_msg "Available Preset Packs:" "$BLUE"
    echo "1) Cream of the Crop (10,000+ presets) - Recommended"
    echo "2) Classic projectM (4,000+ presets)"
    echo "3) Original Milkdrop 2 (500+ presets)"
    echo "4) Milkdrop Texture Pack (textures only)"
    echo ""
    read -p "Select pack to install (1-4): " choice

    cd "$PROJECT_DIR"

    case $choice in
        1)
            print_msg "Installing Cream of the Crop presets..." "$GREEN"
            git clone https://github.com/projectM-visualizer/presets-cream-of-the-crop.git presets-cotc
            print_msg "Installed to: $PROJECT_DIR/presets-cotc" "$GREEN"
            ;;
        2)
            print_msg "Installing Classic projectM presets..." "$GREEN"
            git clone https://github.com/projectM-visualizer/presets-projectm-classic.git presets-classic
            print_msg "Installed to: $PROJECT_DIR/presets-classic" "$GREEN"
            ;;
        3)
            print_msg "Installing Original Milkdrop presets..." "$GREEN"
            git clone https://github.com/projectM-visualizer/presets-milkdrop-original.git presets-milkdrop
            print_msg "Installed to: $PROJECT_DIR/presets-milkdrop" "$GREEN"
            ;;
        4)
            print_msg "Installing Milkdrop Texture Pack..." "$GREEN"
            git clone https://github.com/projectM-visualizer/presets-milkdrop-texture-pack.git textures
            print_msg "Installed to: $PROJECT_DIR/textures" "$GREEN"
            ;;
        *)
            print_msg "Invalid choice!" "$RED"
            ;;
    esac
}

# Function to edit config
edit_config() {
    if command -v nano &> /dev/null; then
        nano "$CONFIG_FILE"
    elif command -v vim &> /dev/null; then
        vim "$CONFIG_FILE"
    else
        print_msg "Opening config in TextEdit..." "$BLUE"
        open -e "$CONFIG_FILE"
    fi
}

# Function to rebuild projectM
rebuild_project() {
    print_msg "Rebuilding projectM..." "$BLUE"
    cd "$BUILD_DIR"
    ninja -j$(sysctl -n hw.ncpu)

    if [ $? -eq 0 ]; then
        print_msg "Build successful!" "$GREEN"
    else
        print_msg "Build failed. Check errors above." "$RED"
    fi
}

# Function to run projectM with presets
run_projectm() {
    local preset_path="$1"
    check_build

    print_msg "Starting projectM Visualizer..." "$GREEN"
    print_msg "Preset path: $preset_path" "$BLUE"
    print_msg "Press CMD+Q to quit" "$YELLOW"

    cd "$BUILD_DIR"
    "$EXECUTABLE" --presetPath "$preset_path"
}

# Main script logic
case "$1" in
    run)
        run_projectm "$PROJECT_DIR/presets/tests"
        ;;
    run-cotc)
        if [ ! -d "$PROJECT_DIR/presets-cotc" ]; then
            print_msg "Cream of the Crop presets not installed." "$RED"
            print_msg "Run '$0 install' to install them." "$YELLOW"
            exit 1
        fi
        run_projectm "$PROJECT_DIR/presets-cotc"
        ;;
    run-custom)
        read -p "Enter preset directory path: " custom_path
        if [ -d "$custom_path" ]; then
            run_projectm "$custom_path"
        else
            print_msg "Directory not found: $custom_path" "$RED"
            exit 1
        fi
        ;;
    install)
        install_presets
        ;;
    config)
        edit_config
        ;;
    rebuild)
        rebuild_project
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        if [ -z "$1" ]; then
            # No arguments - run with default presets
            run_projectm "$PROJECT_DIR/presets/tests"
        else
            print_msg "Unknown option: $1" "$RED"
            show_help
            exit 1
        fi
        ;;
esac