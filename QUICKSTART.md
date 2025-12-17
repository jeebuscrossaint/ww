# ww - Quick Start Guide

Get started with `ww` in 5 minutes!

## Installation

### 1. Install Dependencies

**Arch Linux:**
```bash
sudo pacman -S xmake wayland wayland-protocols ffmpeg libpng libjpeg-turbo libwebp libtiff libjxl
```

**Ubuntu/Debian:**
```bash
sudo apt install libwayland-dev wayland-protocols libavcodec-dev libavformat-dev \
                 libswscale-dev libpng-dev libjpeg-dev libwebp-dev libtiff-dev \
                 libjxl-dev
# Install xmake
curl -fsSL https://xmake.io/shget.text | bash
```

**Fedora:**
```bash
sudo dnf install wayland-devel wayland-protocols-devel ffmpeg-devel \
                 libpng-devel libjpeg-turbo-devel libwebp-devel libtiff-devel \
                 libjxl-devel
# Install xmake
curl -fsSL https://xmake.io/shget.text | bash
```

### 2. Build

```bash
cd ww
xmake
```

### 3. Install (Optional)

```bash
xmake install
# Or to custom location:
xmake install -o ~/.local
```

---

## Basic Usage

### Set a wallpaper

```bash
ww ~/Pictures/wallpaper.jpg
```

That's it! Your wallpaper is now set.

---

## Common Tasks

### 1. Different scaling modes

```bash
# Fill screen (crop if needed)
ww --mode fill ~/Pictures/landscape.jpg

# Center without scaling
ww --mode center ~/Pictures/logo.png

# Stretch (ignore aspect ratio)
ww --mode stretch ~/Pictures/wide.jpg
```

### 2. Solid color background

```bash
ww --color '#282828'
```

### 3. Animated wallpaper

```bash
ww --loop ~/Videos/animated.mp4
```

### 4. Set for specific monitor

```bash
# List available outputs
ww --list-outputs

# Set for specific output
ww -o DP-1 ~/Pictures/wallpaper.png
```

---

## Slideshow Mode

### Basic slideshow

```bash
# Change every 5 minutes (default)
ww -S image1.jpg image2.jpg image3.jpg

# Change every 60 seconds
ww -S -i 60 *.jpg
```

### Load from directory

```bash
# Load all images from directory
ww -S ~/Pictures/wallpapers/

# Scan recursively (all subdirectories)
ww -S -R ~/Pictures/wallpapers/
```

### Random order

```bash
ww -S -r ~/Pictures/wallpapers/
```

---

## Transitions

### Fade between images

```bash
# Default fade (1 second)
ww -S -t fade ~/Pictures/*.jpg

# Slow fade (3 seconds)
ww -S -t fade -d 3.0 ~/Pictures/*.jpg

# Quick fade (0.5 seconds)
ww -S -t fade -d 0.5 ~/Pictures/*.jpg
```

### Slide transitions

```bash
# Slide from right
ww -S -t slide-left ~/Pictures/*.jpg

# Slide from left
ww -S -t slide-right ~/Pictures/*.jpg

# Slide from bottom
ww -S -t slide-up ~/Pictures/*.jpg

# Slide from top
ww -S -t slide-down ~/Pictures/*.jpg
```

---

## The Complete Package

Combine all features for the ultimate slideshow:

```bash
ww -S -R -r -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

This command:
- `-S` - Slideshow mode
- `-R` - Scan directories recursively
- `-r` - Random order
- `-t fade` - Fade transition
- `-d 2.0` - 2-second transition duration
- `-i 300` - Change every 5 minutes
- `~/Pictures/Wallpapers/` - Your wallpaper directory

---

## Tips

### 1. Create an alias

Add to `~/.bashrc` or `~/.zshrc`:

```bash
alias wallpaper='ww -S -R -t fade -d 2.0 -i 300'
```

Then use it:
```bash
wallpaper ~/Pictures/Wallpapers/
```

### 2. Auto-start with your compositor

**Sway:**
Add to `~/.config/sway/config`:
```
exec ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

**Hyprland:**
Add to `~/.config/hypr/hyprland.conf`:
```
exec-once = ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

### 3. Organize your wallpapers

```
~/Pictures/Wallpapers/
├── Nature/
├── Abstract/
├── Minimal/
└── Dark/
```

Then switch themes easily:
```bash
ww -S -R ~/Pictures/Wallpapers/Nature/
ww -S -R ~/Pictures/Wallpapers/Dark/
```

---

## Supported Formats

**Images:** PNG, JPEG, WebP, TIFF, JPEG XL, BMP, TGA, PNM, Farbfeld

**Animated:** GIF, MP4, WebM, Animated WebP

---

## Help

```bash
# Show all options
ww --help

# Show version
ww --version

# List outputs
ww --list-outputs
```

---

## Examples

### Morning routine
```bash
ww -S -R -t fade -d 2.0 -i 600 ~/Pictures/Bright/
```

### Work mode
```bash
ww -S -R -t fade -d 1.0 -i 3600 ~/Pictures/Minimal/
```

### Evening ambiance
```bash
ww -S -R -t fade -d 3.0 -i 900 ~/Pictures/Dark/
```

### Quick preview
```bash
ww -S -R -t fade -d 0.5 -i 5 ~/Pictures/
```

---

## Troubleshooting

**Q: "Failed to initialize" error**

A: Make sure you're running a Wayland compositor (Sway, Hyprland, River, etc.)

**Q: No images found in directory**

A: Use recursive scan: `ww -S -R /path/to/directory`

**Q: Transitions are choppy**

A: Try shorter duration: `-d 0.5` or no transition: `-t none`

**Q: How do I stop a slideshow?**

A: Press `Ctrl+C` in the terminal

---

## Next Steps

- Read `EXAMPLES.md` for more practical examples
- Check `README.md` for detailed documentation
- See `FEATURES.md` for in-depth feature descriptions

---

## Quick Reference Card

```
# Basic
ww image.jpg                    # Set wallpaper
ww --color '#282828'            # Solid color

# Slideshow
ww -S *.jpg                     # Basic slideshow
ww -S -R ~/Pictures/            # Scan directory
ww -S -r *.jpg                  # Random order
ww -S -i 60 *.jpg               # 60 second interval

# Transitions
ww -S -t fade *.jpg             # Fade transition
ww -S -t slide-left *.jpg       # Slide transition
ww -S -t fade -d 2.0 *.jpg      # 2 second fade

# Scaling
ww -m fill image.jpg            # Fill screen
ww -m center image.png          # Center image
ww -m tile pattern.png          # Tile pattern

# Combined
ww -S -R -r -t fade -d 2.0 -i 300 ~/Pictures/
```

---

**You're ready to go!** 🎉

Start with: `ww ~/Pictures/wallpaper.jpg`
