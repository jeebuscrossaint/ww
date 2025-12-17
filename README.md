# ww - Universal Wayland Wallpaper Setter

A fast, optimized, and universal wallpaper setter for Wayland compositors.

## Features

- 🚀 **Fast & Optimized** - Written in C++ with performance in mind
- 🎨 **Universal Format Support** - PNG, JPEG, WebP, GIF, MP4, WebM
- 🖥️ **Multi-Monitor** - Set wallpapers per-output or all at once
- 🔄 **Animated Wallpapers** - Support for GIF and video formats
- 🪟 **Compositor Agnostic** - Works with wlroots-based compositors (sway, Hyprland, etc.)

## Supported Formats

- **Static Images**: PNG, JPEG, WebP
- **Animated**: GIF, MP4, WebM

## Building

### Prerequisites

Install xmake:
```bash
curl -fsSL https://xmake.io/shget.text | bash
```

Or via your package manager:
```bash
# Arch Linux
sudo pacman -S xmake

# Ubuntu/Debian (add PPA first)
sudo add-apt-repository ppa:xmake-io/xmake
sudo apt update
sudo apt install xmake
```

### Dependencies

xmake will automatically handle most dependencies, but you may need system packages:

```bash
# Arch Linux
sudo pacman -S wayland wayland-protocols ffmpeg libpng libjpeg-turbo libwebp

# Ubuntu/Debian
sudo apt install libwayland-dev wayland-protocols libavcodec-dev libavformat-dev \
                 libswscale-dev libpng-dev libjpeg-dev libwebp-dev

# Fedora
sudo dnf install wayland-devel wayland-protocols-devel ffmpeg-devel \
                 libpng-devel libjpeg-turbo-devel libwebp-devel
```

### Build Commands

```bash
# Configure and build
xmake

# Build release version (optimized)
xmake f -m release
xmake

# Build debug version
xmake f -m debug
xmake

# Clean build
xmake clean
xmake

# Install
xmake install

# Install to custom location
xmake install -o /usr/local
```

## Usage

```bash
# Set wallpaper on all outputs
ww /path/to/image.png

# Set wallpaper on specific output
ww -o DP-1 /path/to/image.jpg

# List available outputs
ww --list-outputs

# Set animated wallpaper (looping)
ww --loop /path/to/video.mp4

# Set wallpaper with stretch (don't preserve aspect ratio)
ww --stretch /path/to/image.webp

# Show help
ww --help

# Show version
ww --version
```

## Options

```
-o, --output <name>    Set wallpaper for specific output
-l, --loop             Loop animated wallpapers (GIF/video)
-s, --stretch          Stretch to fill (don't preserve aspect)
-L, --list-outputs     List available outputs
-v, --version          Show version information
-h, --help             Show help message
```

## Performance

Built with performance in mind:
- Zero-copy operations where possible
- SIMD optimizations enabled in release builds
- Link-time optimization (LTO)
- No exceptions/RTTI overhead
- Efficient memory management

## Supported Compositors

Currently supports Wayland compositors that implement:
- `wlr-layer-shell-unstable-v1` (wlroots-based compositors)

Tested on:
- [ ] Sway
- [ ] Hyprland
- [ ] River
- [ ] Wayfire

## Development

```bash
# Run after building
xmake run ww

# Run with arguments
xmake run ww -- --help

# Run tests (when implemented)
xmake test
```

## Project Structure

```
ww/
├── inc/           # Header files
├── src/           # Source files
├── xmake.lua      # Build configuration
└── README.md      # This file
```

## TODO

- [ ] Implement Wayland protocol handling
- [ ] Image decoding (PNG, JPEG, WebP)
- [ ] Video decoding (MP4, WebM via FFmpeg)
- [ ] GIF animation support
- [ ] GPU acceleration for video playback
- [ ] Configuration file support
- [ ] Daemon mode for persistent animated wallpapers
- [ ] Multiple compositor protocol support

## Contributing

Contributions welcome! This project aims to be the fastest and most feature-complete Wayland wallpaper setter.

## License

See LICENSE file for details.

## Acknowledgments

- Inspired by existing tools like swaybg,