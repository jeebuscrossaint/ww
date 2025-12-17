# ww - Usage Examples

Advanced usage patterns and real-world examples for power users.

## Table of Contents

- [Slideshow Patterns](#slideshow-patterns)
- [Multi-Monitor Setups](#multi-monitor-setups)
- [Transition Showcase](#transition-showcase)
- [Workflow Integration](#workflow-integration)
- [Creative Use Cases](#creative-use-cases)

---

## Slideshow Patterns

### Professional Setup

```bash
# Daily rotation with elegant fade
ww -S -R -t fade -d 2.0 -i 600 ~/Pictures/Wallpapers/

# Work environment - minimal distraction
ww -S -R -t fade -d 1.0 -i 3600 ~/Pictures/Wallpapers/Minimal/

# Home office - change with natural light
# Morning (6 AM - run via cron)
ww -S -R -t fade -d 2.0 ~/Pictures/Wallpapers/Bright/

# Evening (6 PM - run via cron)
ww -S -R -t fade -d 3.0 ~/Pictures/Wallpapers/Dark/
```

### Gallery & Presentation

```bash
# Art gallery - slow, random with long fade
ww -S -R -r -t fade -d 4.0 -i 180 ~/Pictures/Art/

# Portfolio showcase - quick preview
ww -S -r -t fade -d 0.5 -i 5 ~/Portfolio/

# Presentation slides with slide transition
ww -S -t slide-left -d 0.8 -i 30 ~/Presentations/slides/

# Ambient background - very long intervals
ww -S -R -t fade -d 5.0 -i 3600 ~/Pictures/Ambient/
```

### Mood-Based Themes

```bash
# Focus mode - single minimal wallpaper, no distraction
ww --mode fill ~/Pictures/minimal-dark.png

# Creative mode - colorful, rapid rotation
ww -S -R -r -t slide-left -d 1.0 -i 60 ~/Pictures/Colorful/

# Relax mode - nature scenes, slow fade
ww -S -R -t fade -d 3.0 -i 900 ~/Pictures/Nature/
```

---

## Multi-Monitor Setups

### Different Wallpapers Per Monitor

```bash
# List available outputs
ww --list-outputs

# Set different wallpapers
ww -o DP-1 ~/Pictures/left-monitor.jpg &
ww -o DP-2 ~/Pictures/center-monitor.jpg &
ww -o HDMI-1 ~/Pictures/right-monitor.jpg &
```

### Synced Slideshows Across Monitors

```bash
# Same slideshow on all outputs (default behavior)
ww -S -R -t fade -d 2.0 ~/Pictures/Wallpapers/

# Different themes per monitor
ww -o DP-1 -S -R ~/Pictures/Tech/ &
ww -o DP-2 -S -R ~/Pictures/Nature/ &
```

### Portrait vs Landscape Optimization

```bash
# Organize by orientation
mkdir -p ~/Pictures/Wallpapers/portrait
mkdir -p ~/Pictures/Wallpapers/landscape

# Set based on monitor orientation
ww -o DP-1 -S -R ~/Pictures/Wallpapers/landscape/  # Landscape monitor
ww -o DP-2 -S -R ~/Pictures/Wallpapers/portrait/   # Portrait monitor
```

---

## Transition Showcase

### Testing All Transitions

```bash
# Create test sequence with 2 contrasting images
IMG1=~/Pictures/dark.jpg
IMG2=~/Pictures/light.jpg

# Fade (smooth crossfade)
ww -S -t fade -d 2.0 -i 5 "$IMG1" "$IMG2"

# Slide transitions (directional)
ww -S -t slide-left -d 1.5 -i 5 "$IMG1" "$IMG2"
ww -S -t slide-right -d 1.5 -i 5 "$IMG1" "$IMG2"
ww -S -t slide-up -d 1.2 -i 5 "$IMG1" "$IMG2"
ww -S -t slide-down -d 1.2 -i 5 "$IMG1" "$IMG2"

# No transition (instant)
ww -S -t none -i 3 "$IMG1" "$IMG2"
```

### Transition Speed Comparison

```bash
# Ultra-fast (0.3s) - snappy, modern feel
ww -S -t fade -d 0.3 -i 10 ~/Pictures/*.jpg

# Standard (1.0s) - balanced
ww -S -t fade -d 1.0 -i 10 ~/Pictures/*.jpg

# Slow (3.0s) - dramatic, cinematic
ww -S -t fade -d 3.0 -i 10 ~/Pictures/*.jpg

# Very slow (5.0s) - ambient, meditative
ww -S -t fade -d 5.0 -i 10 ~/Pictures/*.jpg
```

### Context-Appropriate Transitions

```bash
# Photo gallery - fade for elegance
ww -S -R -t fade -d 2.0 ~/Pictures/Photos/

# UI mockups - slide for context
ww -S -t slide-left -d 1.0 ~/Design/mockups/

# Quick preview - none for speed
ww -S -t none ~/Pictures/test/

# Ambient display - slow fade for calm
ww -S -t fade -d 4.0 -i 600 ~/Pictures/Ambient/
```

---

## Workflow Integration

### Compositor Auto-Start

**Sway** (`~/.config/sway/config`):
```
exec ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

**Hyprland** (`~/.config/hypr/hyprland.conf`):
```
exec-once = ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
```

**River** (`~/.config/river/init`):
```sh
ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/ &
```

### Shell Aliases

Add to `~/.bashrc` or `~/.zshrc`:

```bash
# Quick wallpaper commands
alias wp='ww'
alias wps='ww -S -R -t fade -d 2.0 -i 300'  # Standard slideshow
alias wpr='ww -S -R -r -t fade -d 1.5 -i 120'  # Random slideshow
alias wpq='ww -S -R -t fade -d 0.5 -i 5'  # Quick preview

# Theme switching
alias wp-dark='ww -S -R ~/Pictures/Wallpapers/Dark/'
alias wp-light='ww -S -R ~/Pictures/Wallpapers/Light/'
alias wp-minimal='ww -S -R ~/Pictures/Wallpapers/Minimal/'
alias wp-nature='ww -S -R ~/Pictures/Wallpapers/Nature/'

# Quick solid colors
alias wp-black='ww --color "#000000"'
alias wp-gray='ww --color "#282828"'
alias wp-nord='ww --color "#2E3440"'
```

### Time-Based Wallpapers (Cron)

Edit with `crontab -e`:

```cron
# Morning wallpapers (6 AM)
0 6 * * * ww -S -R ~/Pictures/Wallpapers/Morning/

# Afternoon wallpapers (12 PM)
0 12 * * * ww -S -R ~/Pictures/Wallpapers/Bright/

# Evening wallpapers (6 PM)
0 18 * * * ww -S -R ~/Pictures/Wallpapers/Evening/

# Night wallpapers (10 PM)
0 22 * * * ww -S -R ~/Pictures/Wallpapers/Dark/
```

### Systemd Timer (Alternative to Cron)

**Service file** (`~/.config/systemd/user/wallpaper.service`):
```ini
[Unit]
Description=Wallpaper Slideshow

[Service]
Type=simple
ExecStart=/usr/local/bin/ww -S -R -t fade -d 2.0 -i 300 %h/Pictures/Wallpapers/
Restart=on-failure

[Install]
WantedBy=default.target
```

Enable and start:
```bash
systemctl --user enable --now wallpaper.service
```

---

## Creative Use Cases

### Dynamic Desktop Environment

```bash
# Organize wallpapers by mood/context
~/Pictures/Wallpapers/
├── focus/        # Minimal, distraction-free
├── creative/     # Colorful, inspiring
├── relax/        # Calm, nature scenes
└── energetic/    # Bright, vibrant

# Switch based on activity
ww -S -R ~/Pictures/Wallpapers/focus/      # Deep work
ww -S -R ~/Pictures/Wallpapers/creative/   # Design/art
ww -S -R ~/Pictures/Wallpapers/relax/      # Breaks
```

### Photography Portfolio Display

```bash
# Showcase your work on idle displays
ww -S -r -t fade -d 3.0 -i 30 ~/Photography/Portfolio/

# Client presentation mode
ww -S -t slide-left -d 1.0 -i 15 ~/Photography/Client-Preview/

# Exhibition mode - slow, random
ww -S -r -t fade -d 5.0 -i 120 ~/Photography/Exhibition/
```

### Digital Signage

```bash
# Restaurant menu boards
ww -S -t slide-left -d 1.0 -i 10 ~/Signage/Menu/

# Retail displays
ww -S -t fade -d 2.0 -i 15 ~/Signage/Products/

# Information kiosk
ww -S -t fade -d 3.0 -i 20 ~/Signage/Info/

# Event displays
ww -S -t slide-left -d 1.5 -i 8 ~/Events/Sponsors/
```

### Screensaver Mode

```bash
# Rapid random transitions for visual interest
ww -S -r -t fade -d 0.3 -i 5 ~/Pictures/Screensaver/

# Slow ambient mode
ww -S -r -t fade -d 4.0 -i 30 ~/Pictures/Ambient/
```

### Testing & Development

```bash
# Quick format test
ww -S -i 2 test.png test.jpg test.webp test.jxl test.tiff

# Transition development testing
for t in fade slide-left slide-right slide-up slide-down none; do
    echo "Testing: $t"
    ww -S -t $t -d 1.0 -i 3 img1.jpg img2.jpg
    sleep 10
done

# Performance testing
time ww -S -t fade -d 1.0 -i 2 ~/Pictures/4k/*.jpg
```

---

## Advanced Techniques

### Conditional Wallpapers

```bash
#!/bin/bash
# Set wallpaper based on time of day

hour=$(date +%H)

if [ $hour -ge 6 ] && [ $hour -lt 12 ]; then
    ww -S -R ~/Pictures/Wallpapers/Morning/
elif [ $hour -ge 12 ] && [ $hour -lt 18 ]; then
    ww -S -R ~/Pictures/Wallpapers/Day/
elif [ $hour -ge 18 ] && [ $hour -lt 22 ]; then
    ww -S -R ~/Pictures/Wallpapers/Evening/
else
    ww -S -R ~/Pictures/Wallpapers/Night/
fi
```

### Battery-Aware Performance

```bash
#!/bin/bash
# Adjust transition quality based on battery

if [ -f /sys/class/power_supply/BAT0/status ]; then
    status=$(cat /sys/class/power_supply/BAT0/status)
    
    if [ "$status" = "Discharging" ]; then
        # Battery mode - no transitions, longer intervals
        ww -S -R -t none -i 600 ~/Pictures/Wallpapers/
    else
        # AC power - full quality
        ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
    fi
else
    # Desktop - full quality
    ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/Wallpapers/
fi
```

### Directory Organization Best Practices

```bash
# Organize by resolution
~/Pictures/Wallpapers/
├── 1080p/
├── 1440p/
└── 4k/

# Organize by orientation
~/Pictures/Wallpapers/
├── landscape/
├── portrait/
└── ultrawide/

# Organize by category
~/Pictures/Wallpapers/
├── Abstract/
├── Anime/
├── Art/
├── Minimal/
├── Nature/
├── Space/
└── Urban/

# Use appropriate directories
ww -S -R ~/Pictures/Wallpapers/4k/         # 4K display
ww -S -R ~/Pictures/Wallpapers/landscape/  # Landscape monitor
ww -S -R ~/Pictures/Wallpapers/Nature/     # Nature theme
```

---

## Performance Tips

### High-Resolution Displays

```bash
# For 4K displays, use longer transitions
ww -S -t fade -d 2.0 -i 300 ~/Pictures/4k/

# Or disable transitions for maximum performance
ww -S -t none -i 300 ~/Pictures/4k/

# Longer intervals reduce CPU usage
ww -S -t fade -d 1.0 -i 600 ~/Pictures/4k/
```

### Large Image Collections

```bash
# Recursive scan is one-time cost at startup
ww -S -R -t fade -d 2.0 -i 300 ~/Pictures/

# Images loaded on-demand, not all at once
# Safe for directories with thousands of images

# Random mode has negligible overhead
ww -S -R -r -t fade -d 2.0 ~/Pictures/
```

### Low-End Systems

```bash
# Disable transitions
ww -S -R -t none ~/Pictures/Wallpapers/

# Use longer intervals
ww -S -R -t none -i 600 ~/Pictures/Wallpapers/

# Smaller images/resolutions
ww -S -R -t none ~/Pictures/Wallpapers/1080p/
```

---

## Troubleshooting Examples

### Testing Specific Formats

```bash
# Test individual format support
ww test.png    # PNG
ww test.jpg    # JPEG
ww test.webp   # WebP
ww test.jxl    # JPEG XL
ww test.tiff   # TIFF

# Test animated formats
ww --loop test.gif     # GIF
ww --loop test.mp4     # MP4
ww --loop test.webm    # WebM
```

### Debugging Directory Scans

```bash
# Verify directory has supported images
ls ~/Pictures/Wallpapers/

# Test non-recursive first
ww -S ~/Pictures/Wallpapers/

# Then test recursive
ww -S -R ~/Pictures/Wallpapers/

# Check permissions
ls -la ~/Pictures/Wallpapers/
```

### Transition Issues

```bash
# Try shorter duration
ww -S -t fade -d 0.5 ~/Pictures/*.jpg

# Try different transition type
ww -S -t none ~/Pictures/*.jpg

# Test with just 2 images
ww -S -t fade -d 1.0 -i 5 img1.jpg img2.jpg
```

---

## More Information

- Run `ww --help` for all options
- Check `README.md` for installation and build instructions
- Visit the repository for updates and issue tracking