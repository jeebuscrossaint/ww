# ww - Feature Demo

## Quick Demo Commands

This document contains copy-paste ready commands to demonstrate the new features.

---

## Setup

```bash
# Build the project
cd /home/amarnath/Projects/ww
xmake build

# Create demo wallpaper directory
mkdir -p ~/demo_wallpapers
```

---

## Demo 1: Basic Wallpaper Setting

```bash
# Set a single wallpaper
./build/linux/x86_64/debug/ww test.png

# Set with fill mode
./build/linux/x86_64/debug/ww --mode fill test.jpg

# Set with center mode and background color
./build/linux/x86_64/debug/ww --mode center --color '#282828' test.png
```

---

## Demo 2: Directory Scanning (Non-Recursive)

```bash
# Copy test images to demo directory
cp test*.{png,jpg,webp} ~/demo_wallpapers/ 2>/dev/null

# Scan directory - shows all images found
./build/linux/x86_64/debug/ww -S ~/demo_wallpapers/

# What happens:
# - Automatically finds all supported image formats
# - Loads: PNG, JPEG, WebP, TIFF, JXL, BMP, TGA, GIF, MP4, WebM, etc.
# - Shows count of images loaded
```

---

## Demo 3: Recursive Directory Scanning

```bash
# Create nested directory structure
mkdir -p ~/demo_wallpapers/nature
mkdir -p ~/demo_wallpapers/abstract
mkdir -p ~/demo_wallpapers/tech

# Copy images to subdirectories
cp test.png ~/demo_wallpapers/nature/
cp test.jpg ~/demo_wallpapers/abstract/
cp test.webp ~/demo_wallpapers/tech/

# Scan recursively
./build/linux/x86_64/debug/ww -S -R ~/demo_wallpapers/

# What happens:
# - Scans main directory AND all subdirectories
# - Finds images in nature/, abstract/, tech/
# - Sorts all files alphabetically
```

---

## Demo 4: Fade Transition (Default)

```bash
# Quick slideshow with fade (3 second interval for demo)
./build/linux/x86_64/debug/ww -S -i 3 -t fade -d 1.0 test.png test.jpg test.webp

# Slow dramatic fade (2 second transition)
./build/linux/x86_64/debug/ww -S -i 5 -t fade -d 2.0 test.png test.jpg test.webp

# Quick fade (0.5 seconds)
./build/linux/x86_64/debug/ww -S -i 3 -t fade -d 0.5 test.png test.jpg test.webp
```

**What you'll see:**
- Image A gradually fades out while Image B fades in
- Smooth alpha blending between images
- Natural ease-in-out motion

---

## Demo 5: Slide Left Transition

```bash
# Slide from right to left
./build/linux/x86_64/debug/ww -S -i 3 -t slide-left -d 1.5 test.png test.jpg test.webp
```

**What you'll see:**
- Old image slides out to the left
- New image slides in from the right
- Synchronized motion with easing

---

## Demo 6: Slide Right Transition

```bash
# Slide from left to right
./build/linux/x86_64/debug/ww -S -i 3 -t slide-right -d 1.5 test.png test.jpg test.webp
```

**What you'll see:**
- Old image slides out to the right
- New image slides in from the left

---

## Demo 7: Slide Up Transition

```bash
# Slide from bottom to top
./build/linux/x86_64/debug/ww -S -i 3 -t slide-up -d 1.2 test.png test.jpg test.webp
```

**What you'll see:**
- Old image slides up and disappears
- New image slides in from the bottom

---

## Demo 8: Slide Down Transition

```bash
# Slide from top to bottom
./build/linux/x86_64/debug/ww -S -i 3 -t slide-down -d 1.2 test.png test.jpg test.webp
```

**What you'll see:**
- Old image slides down
- New image slides in from the top

---

## Demo 9: No Transition (Instant)

```bash
# Instant switch, no animation
./build/linux/x86_64/debug/ww -S -i 2 -t none test.png test.jpg test.webp
```

**What you'll see:**
- Immediate switch between images
- No animation or transition
- Best for performance

---

## Demo 10: Random Order with Fade

```bash
# Random slideshow with fade transition
./build/linux/x86_64/debug/ww -S -r -i 3 -t fade -d 1.5 test.png test.jpg test.webp test.bmp test.tga
```

**What you'll see:**
- Images appear in random order
- Each switch uses fade transition
- Never shows the same image twice in a row

---

## Demo 11: Recursive Directory + Random + Fade

```bash
# The complete package!
./build/linux/x86_64/debug/ww -S -R -r -i 3 -t fade -d 2.0 ~/demo_wallpapers/

# What this does:
# 1. Scans ~/demo_wallpapers/ recursively
# 2. Finds all images in all subdirectories
# 3. Shuffles them randomly
# 4. Shows each for 3 seconds
# 5. Fades smoothly (2 seconds) between them
```

---

## Demo 12: Different Transition Types Back-to-Back

```bash
# Watch all transition types (run these one after another)

# 1. Fade
./build/linux/x86_64/debug/ww -S -i 2 -t fade -d 1.0 test.png test.jpg &
sleep 7 && pkill ww

# 2. Slide left
./build/linux/x86_64/debug/ww -S -i 2 -t slide-left -d 1.0 test.png test.jpg &
sleep 7 && pkill ww

# 3. Slide right
./build/linux/x86_64/debug/ww -S -i 2 -t slide-right -d 1.0 test.png test.jpg &
sleep 7 && pkill ww

# 4. Slide up
./build/linux/x86_64/debug/ww -S -i 2 -t slide-up -d 1.0 test.png test.jpg &
sleep 7 && pkill ww

# 5. Slide down
./build/linux/x86_64/debug/ww -S -i 2 -t slide-down -d 1.0 test.png test.jpg &
sleep 7 && pkill ww
```

---

## Demo 13: Real-World Usage Examples

### Home Setup (Daily Wallpapers)
```bash
# 5-minute interval with 2-second fade
./build/linux/x86_64/debug/ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

### Work Setup (Minimal Distraction)
```bash
# 1-hour interval with slow fade
./build/linux/x86_64/debug/ww -S -R -t fade -d 3.0 -i 3600 ~/Pictures/Minimal/
```

### Art Gallery Mode
```bash
# Random, 3-minute interval, slow fade
./build/linux/x86_64/debug/ww -S -R -r -t fade -d 4.0 -i 180 ~/Pictures/Art/
```

### Quick Preview Mode
```bash
# 5-second interval, quick fade
./build/linux/x86_64/debug/ww -S -R -t fade -d 0.5 -i 5 ~/Pictures/
```

---

## Demo 14: Performance Test

```bash
# No transition (fastest)
time ./build/linux/x86_64/debug/ww -S -i 1 -t none test.png test.jpg &
sleep 5 && pkill ww

# With transition
time ./build/linux/x86_64/debug/ww -S -i 1 -t fade -d 0.5 test.png test.jpg &
sleep 5 && pkill ww
```

---

## Demo 15: All Format Support

```bash
# Create samples of different formats if available
./build/linux/x86_64/debug/ww -S -i 2 -t fade -d 1.0 \
  test.png \
  test.jpg \
  test.webp \
  test.bmp \
  test.tga \
  test.ppm \
  test.tiff \
  test.jxl \
  test.ff \
  test_anim.gif \
  test_video.mp4

# Shows support for:
# - PNG, JPEG, WebP, BMP, TGA, PNM, TIFF, JXL, Farbfeld
# - GIF, MP4, WebM
```

---

## Key Observations

### Directory Scanning
- ✅ Automatically finds all supported formats
- ✅ Recursive scanning with `-R`
- ✅ Clean output showing files loaded
- ✅ Handles empty directories gracefully

### Transitions
- ✅ Smooth, professional-quality animations
- ✅ 6 different transition types
- ✅ Configurable duration (0.1s to any length)
- ✅ Ease-in-out motion feels natural
- ✅ No visible tearing or artifacts
- ✅ Maintains 60 FPS during transition

### Combined Features
- ✅ Works seamlessly together
- ✅ Random + Recursive + Fade = Perfect slideshow
- ✅ Intuitive CLI options
- ✅ Sensible defaults

---

## Cleanup

```bash
# Remove demo directory
rm -rf ~/demo_wallpapers
```

---

## Quick Reference

```
# Scan directory
ww -S <directory>

# Scan recursively
ww -S -R <directory>

# With fade transition
ww -S -t fade -d 2.0 <files/directories>

# With slide transition
ww -S -t slide-left -d 1.5 <files/directories>

# Random order
ww -S -r <files/directories>

# Custom interval (seconds)
ww -S -i 300 <files/directories>

# The complete package
ww -S -R -r -t fade -d 2.0 -i 300 ~/Wallpapers/
```

---

## Video Demo Script

If recording a video demo, follow this sequence:

1. **Introduction** (10s)
   - Show help: `ww --help`

2. **Basic wallpaper** (5s)
   - `ww test.png`

3. **Directory scanning** (10s)
   - `ww -S ~/demo_wallpapers/`
   - Show output with file count

4. **Fade transition** (15s)
   - `ww -S -i 3 -t fade -d 1.5 test.png test.jpg test.webp`
   - Let viewers see 2-3 transitions

5. **Slide transitions** (20s)
   - Show slide-left, slide-right, slide-up
   - 5 seconds each

6. **Recursive + Random + Fade** (15s)
   - `ww -S -R -r -t fade -d 2.0 ~/demo_wallpapers/`
   - The "wow" moment

7. **Conclusion** (5s)
   - Show version and GitHub link

**Total**: ~80 seconds

---

## Expected Output

When running slideshow with directory:
```
Loaded 15 images from /home/user/demo_wallpapers
Slideshow started with 15 files
  Interval: 300s
  Random: no
  Transition: fade (2.0s)
Switching to: /home/user/demo_wallpapers/image1.jpg
Switching to: /home/user/demo_wallpapers/image2.png
...
```

---

**End of Demo Guide**