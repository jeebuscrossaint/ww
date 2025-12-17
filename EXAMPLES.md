# ww - Usage Examples

This document provides practical examples of using `ww` wallpaper setter.

## Table of Contents

- [Basic Usage](#basic-usage)
- [Directory Scanning](#directory-scanning)
- [Slideshow Mode](#slideshow-mode)
- [Transitions](#transitions)
- [Advanced Examples](#advanced-examples)

## Basic Usage

### Set a single wallpaper

```bash
# Simple wallpaper
ww ~/Pictures/wallpaper.png

# Specific output
ww -o DP-1 ~/Pictures/wallpaper.jpg

# With scaling mode
ww --mode fill ~/Pictures/landscape.jpg
ww --mode center ~/Pictures/logo.png
```

### Solid colors

```bash
# Set solid color background
ww --color '#282828'
ww --color '#FF5733'
ww --color '#1E1E2EFF'  # With alpha channel
```

### Animated wallpapers

```bash
# Loop a video
ww --loop ~/Videos/animated.mp4

# Play GIF
ww --loop ~/Pictures/animated.gif
```

## Directory Scanning

### Scan a directory

```bash
# Scan directory (non-recursive)
ww -S ~/Pictures/wallpapers/

# The above will find all supported image formats in the directory:
# PNG, JPEG, WebP, TIFF, JXL, BMP, TGA, PNM, Farbfeld, GIF, MP4, WebM
```

### Recursive scanning

```bash
# Scan directory and all subdirectories
ww -S -R ~/Pictures/wallpapers/

# Example directory structure:
# ~/Pictures/wallpapers/
# ├── nature/
# │   ├── forest.jpg
# │   └── ocean.png
# ├── abstract/
# │   ├── colors.webp
# │   └── shapes.png
# └── urban/
#     └── city.jpg
#
# All 5 images will be loaded with -R flag
```

### Mix files and directories

```bash
# Combine explicit files with directory scanning
ww -S ~/Pictures/favorite.jpg ~/Pictures/wallpapers/

# With recursive scanning
ww -S -R ~/Pictures/favorites/ ~/Pictures/wallpapers/ ~/Downloads/bg.png
```

## Slideshow Mode

### Basic slideshow

```bash
# Default interval (5 minutes)
ww -S image1.jpg image2.png image3.webp

# Custom interval (60 seconds)
ww -S -i 60 *.jpg

# Short interval for testing (10 seconds)
ww -S -i 10 ~/Pictures/*.png
```

### Random order

```bash
# Shuffle the images
ww -S -r ~/Pictures/wallpapers/

# Random with custom interval
ww -S -r -i 120 ~/Pictures/*.jpg
```

### Directory slideshow

```bash
# Slideshow from directory (alphabetical order)
ww -S ~/Pictures/wallpapers/

# Random slideshow from recursive scan
ww -S -R -r ~/Pictures/

# Specific subdirectory patterns
ww -S ~/Pictures/nature/ ~/Pictures/abstract/
```

## Transitions

### Fade transition (default)

```bash
# Default fade (1 second)
ww -S -t fade ~/Pictures/wallpapers/

# Slow fade (3 seconds)
ww -S -t fade -d 3.0 ~/Pictures/*.jpg

# Quick fade (0.5 seconds)
ww -S -t fade -d 0.5 -i 30 ~/Pictures/wallpapers/
```

### Slide transitions

```bash
# Slide from right to left
ww -S -t slide-left -d 1.5 ~/Pictures/*.png

# Slide from left to right
ww -S -t slide-right -d 1.0 ~/Pictures/wallpapers/

# Slide from bottom to top
ww -S -t slide-up -d 1.2 ~/Pictures/*.jpg

# Slide from top to bottom
ww -S -t slide-down -d 0.8 ~/Pictures/wallpapers/
```

### No transition

```bash
# Instant switch, no transition
ww -S -t none -i 60 ~/Pictures/*.jpg
```

## Advanced Examples

### Professional slideshow setup

```bash
# Elegant fade slideshow every 10 minutes
ww -S -R -t fade -d 2.0 -i 600 ~/Pictures/Wallpapers/

# Gallery-style rapid slideshow with quick fade
ww -S -r -t fade -d 0.3 -i 5 ~/Pictures/Portfolio/
```

### Event/mood-based wallpapers

```bash
# Morning wallpapers - bright and energetic
ww -S -R -t slide-right -d 1.5 -i 300 ~/Pictures/Wallpapers/Morning/

# Evening wallpapers - calm and dark
ww -S -R -t fade -d 3.0 -i 600 ~/Pictures/Wallpapers/Evening/

# Work wallpapers - minimal and focused
ww -S -t fade -d 1.0 -i 7200 ~/Pictures/Wallpapers/Minimal/
```

### Multi-monitor setup

```bash
# Different wallpaper per output
ww -o DP-1 ~/Pictures/left.jpg &
ww -o DP-2 ~/Pictures/right.jpg &

# Slideshow on specific output
ww -o HDMI-1 -S -R ~/Pictures/wallpapers/
```

### Performance-optimized

```bash
# No transition for better performance
ww -S -t none -i 300 ~/Pictures/4k/

# Quick transitions with longer intervals
ww -S -t fade -d 0.5 -i 600 ~/Pictures/wallpapers/
```

### Combined with scaling modes

```bash
# Fill mode with fade transitions
ww -S -m fill -t fade -d 2.0 ~/Pictures/*.jpg

# Center mode with letterbox color and fade
ww -S -m center -c '#1E1E2E' -t fade -d 1.5 ~/Pictures/logos/

# Tile mode slideshow (for patterns)
ww -S -m tile -t fade -d 1.0 ~/Pictures/patterns/
```

### Testing transitions

```bash
# Test all transition types with short intervals
ww -S -t fade -d 2.0 -i 5 test1.jpg test2.jpg test3.jpg
ww -S -t slide-left -d 1.5 -i 5 test1.jpg test2.jpg test3.jpg
ww -S -t slide-right -d 1.5 -i 5 test1.jpg test2.jpg test3.jpg
ww -S -t slide-up -d 1.0 -i 5 test1.jpg test2.jpg test3.jpg
ww -S -t slide-down -d 1.0 -i 5 test1.jpg test2.jpg test3.jpg
```

### Creative uses

```bash
# Art gallery mode - slow fade between artworks
ww -S -R -r -t fade -d 4.0 -i 180 ~/Pictures/Art/

# Screensaver mode - rapid random transitions
ww -S -r -t fade -d 0.2 -i 3 ~/Pictures/Screensaver/

# Presentation mode - manual-like slide transitions
ww -S -t slide-left -d 0.8 -i 30 ~/Presentations/slides/

# Ambient mode - very long intervals with gentle fades
ww -S -t fade -d 5.0 -i 3600 ~/Pictures/Ambient/
```

## Tips and Tricks

### 1. Organize your wallpapers

```bash
~/Pictures/Wallpapers/
├── Nature/
├── Abstract/
├── Minimal/
├── Dark/
└── Bright/

# Then use:
ww -S -R ~/Pictures/Wallpapers/Nature/  # Nature theme
ww -S -R ~/Pictures/Wallpapers/Dark/    # Dark theme
```

### 2. Use shell aliases

Add to your `.bashrc` or `.zshrc`:

```bash
# Quick slideshow with fade
alias ww-slideshow='ww -S -R -t fade -d 2.0 -i 300'

# Random rapid slideshow
alias ww-random='ww -S -R -r -t fade -d 1.0 -i 60'

# Work wallpapers
alias ww-work='ww -S -R -t fade -d 1.5 -i 600 ~/Pictures/Wallpapers/Work/'

# Evening mode
alias ww-evening='ww -S -R -t fade -d 3.0 -i 900 ~/Pictures/Wallpapers/Dark/'
```

### 3. Combine with other tools

```bash
# Start with system startup (add to your compositor config)
exec ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/

# Time-based wallpapers (using cron or systemd timers)
# Morning (6 AM)
0 6 * * * ww -S -R ~/Pictures/Wallpapers/Morning/
# Evening (6 PM)
0 18 * * * ww -S -R ~/Pictures/Wallpapers/Evening/
```

### 4. Performance considerations

- For 4K displays, consider longer transition durations (2-3 seconds)
- Use `none` transition for very large images or slower systems
- Recursive scanning of large directories may take time initially
- Images are loaded on-demand during slideshow for memory efficiency

### 5. Supported format combinations

```bash
# Mix different formats freely
ww -S *.png *.jpg *.webp *.jxl *.gif

# Include videos in slideshow (will play once per interval)
ww -S -i 30 *.mp4 *.webm *.gif

# Recursively find all formats
ww -S -R ~/Pictures/  # Automatically finds all supported formats
```

## Troubleshooting

### Directory has no images

```bash
# Error: No supported image files found in directory
# Solution: Check if the directory contains supported formats
ls ~/Pictures/wallpapers/  # Verify files exist
ww -S -R ~/Pictures/wallpapers/  # Try recursive scan
```

### Transitions not smooth

```bash
# Try shorter transition duration
ww -S -t fade -d 0.5 ~/Pictures/*.jpg

# Or use no transition
ww -S -t none ~/Pictures/*.jpg
```

### Slideshow too fast/slow

```bash
# Adjust interval (-i flag in seconds)
ww -S -i 60 ~/Pictures/*.jpg    # 1 minute
ww -S -i 300 ~/Pictures/*.jpg   # 5 minutes
ww -S -i 3600 ~/Pictures/*.jpg  # 1 hour
```

## More Information

- See `ww --help` for all options
- Check `README.md` for installation and building instructions
- Visit the project repository for latest updates
