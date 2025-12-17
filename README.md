# ww - Universal Wayland Wallpaper Setter

A fast, optimized, and universal wallpaper setter for Wayland compositors.

## Features

- 🚀 **Fast & Optimized** - Written in C++ with performance in mind
- 🎨 **Universal Format Support** - PNG, JPEG, WebP, TIFF, JXL, BMP, TGA, PNM, Farbfeld, GIF, MP4, WebM
- 🖥️ **Multi-Monitor** - Set wallpapers per-output or all at once
- 🔄 **Animated Wallpapers** - Support for GIF and video formats
- 🎬 **Slideshow Mode** - Automatic slideshow with directory scanning and transitions
- ✨ **Smooth Transitions** - Fade and slide effects between wallpapers
- 📁 **Directory Scanning** - Recursively scan folders for images
- 🪟 **Compositor Agnostic** - Works with wlroots-based compositors (sway, Hyprland, etc.)

## Supported Formats

- **Static Images**: PNG, JPEG, WebP, TIFF/TIF, JPEG XL (JXL), BMP, TGA, PNM/PBM/PGM/PPM, Farbfeld
- **Animated**: GIF, MP4, WebM, Animated WebP

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
                 libswscale-dev libpng-dev libjpeg-dev libwebp-dev libtiff-dev \
                 libjxl-dev

# Fedora
sudo dnf install wayland-devel wayland-protocols-devel ffmpeg-devel \
                 libpng-devel libjpeg-turbo-devel libwebp-devel libtiff-devel \
                 libjxl-devel
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

### Basic Usage

```bash
# Set wallpaper on all outputs
ww /path/to/image.png

# Set wallpaper on specific output
ww -o DP-1 /path/to/image.jpg

# List available outputs
ww --list-outputs

# Set animated wallpaper (looping)
ww --loop /path/to/video.mp4

# Set wallpaper with different scaling modes
ww --mode fill /path/to/image.jpg
ww --mode center --color '#282828' /path/to/logo.png

# Set solid color background
ww --color '#FF5733'
```

### Slideshow Mode

```bash
# Basic slideshow with multiple files (5 minute interval by default)
ww -S image1.jpg image2.png image3.webp

# Slideshow with custom interval (60 seconds)
ww -S -i 60 ~/wallpapers/*.jpg

# Random order slideshow
ww -S -r -i 120 ~/wallpapers/*.png

# Scan directory for images
ww -S ~/wallpapers/

# Recursively scan directory and subdirectories
ww -S -R ~/wallpapers/

# Slideshow with fade transition (2 second fade)
ww -S -t fade -d 2.0 ~/wallpapers/*.png

# Slideshow with fade at 60 FPS (smoother)
ww -S -t fade -d 2.0 -f 60 ~/wallpapers/*.png

# Slideshow with slide transition
ww -S -t slide-left -d 1.5 ~/wallpapers/
```

## Options

```
-o, --output <name>      Set wallpaper for specific output
-m, --mode <mode>        Scaling mode: fit, fill, stretch, center, tile (default: fit)
-c, --color <#RRGGBB>    Solid color background or letterbox color
-l, --loop               Loop animated wallpapers (GIF/video)
-S, --slideshow          Slideshow mode (multiple files/directories)
-i, --interval <sec>     Slideshow interval in seconds (default: 300)
-r, --random             Random slideshow order
-R, --recursive          Scan directories recursively
-t, --transition <type>  Transition effect: none, fade, slide-left, slide-right,
                         slide-up, slide-down (default: fade)
-d, --duration <sec>     Transition duration in seconds (default: 1.0)
-f, --fps <fps>          Transition frame rate (default: 30, max: 120)
-L, --list-outputs       List available outputs
-v, --version            Show version information
-h, --help               Show help message
```

### Scaling Modes

- **fit** - Scale to fit with letterboxing (preserves aspect ratio, default)
- **fill** - Scale to fill, crop if needed (preserves aspect ratio)
- **stretch** - Stretch to fill, ignore aspect ratio
- **center** - No scaling, center image
- **tile** - Repeat image to fill screen

### Transition Effects

**Basic:**
- **none** - Instant switch, no transition
- **fade** - Smooth crossfade between images

**Slide:**
- **slide-left** - Slide old image left, new image from right
- **slide-right** - Slide old image right, new image from left
- **slide-up** - Slide old image up, new image from bottom
- **slide-down** - Slide old image down, new image from top

**Zoom:**
- **zoom-in** - Zoom in while fading to new image
- **zoom-out** - Zoom out while fading to new image

**Circle:**
- **circle-open** - Circular reveal from center outward
- **circle-close** - Circular collapse from edges to center

**Wipe:**
- **wipe-left** - Curtain wipe from left to right
- **wipe-right** - Curtain wipe from right to left
- **wipe-up** - Curtain wipe from bottom to top
- **wipe-down** - Curtain wipe from top to bottom

**Effects:**
- **dissolve** - Random pixel dissolve effect
- **pixelate** - Pixelate transition with mosaic effect

### Transition Performance

Control the smoothness vs performance trade-off with the FPS option:
- **15 FPS** - Lower CPU usage, good for older systems
- **30 FPS** (default) - Balanced smoothness and performance
- **60 FPS** - Silky smooth transitions, higher CPU usage

```bash
# Smooth 60 FPS fade
ww -S -t fade -d 2.0 -f 60 ~/wallpapers/

# Performance mode at 15 FPS
ww -S -t fade -d 2.0 -f 15 ~/wallpapers/
```

## Image Quality

**High-Quality Scaling:**
- **Bicubic interpolation** for smooth, high-quality scaling
- **Bilinear fallback** for extreme scale factors
- Much better quality than nearest-neighbor scaling
- No pixelation or jagged edges

**Performance:**
- Optimized scaling algorithms
- Smart scaling selection based on scale factor
- Efficient memory usage

## Performance

Built with performance in mind:
- **Bicubic/bilinear scaling** for best image quality
- Zero-copy operations where possible
- SIMD optimizations enabled in release builds
- Link-time optimization (LTO)
- No exceptions/RTTI overhead
- Efficient memory management
- **60 FPS transitions** with configurable frame rate
- GPU acceleration planned (see GPU_ACCELERATION.md)

### Transition Performance
- 1920x1080 @ 60 FPS: ~5-10% CPU
- 2560x1600 @ 60 FPS: ~10-15% CPU
- 3840x2160 @ 60 FPS: ~40-50% CPU
- Adjustable FPS for performance tuning

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

- [x] Implement Wayland protocol handling
- [x] Image decoding (PNG, JPEG, WebP, TIFF, JXL, BMP, TGA, PNM, Farbfeld)
- [x] Video decoding (MP4, WebM via FFmpeg)
- [x] GIF animation support
- [x] Slideshow mode with transitions
- [x] Directory scanning (recursive)
- [x] Multiple scaling modes
- [x] High-quality bicubic/bilinear scaling
- [x] 14+ transition effects (fade, slide, zoom, circle, wipe, dissolve, pixelate)
- [x] Configurable transition FPS
- [ ] GPU acceleration (Vulkan/OpenGL ES) - see GPU_ACCELERATION.md
- [ ] Configuration file support
- [ ] EXIF orientation support
- [ ] Hardware video decoding (VAAPI/VDPAU)
- [ ] Time-based wallpaper switching
- [ ] Watch mode for file/directory changes
- [ ] Playlist file support
- [ ] Image filters (blur, brightness, saturation)

## Contributing

Contributions welcome! This project aims to be the fastest and most feature-complete Wayland wallpaper setter.

## License

See LICENSE file for details.

## Acknowledgments

- Inspired by existing tools like swaybg,