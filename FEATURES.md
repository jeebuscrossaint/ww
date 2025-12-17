# ww - New Features Implementation

This document describes the newly implemented features for the `ww` Wayland wallpaper setter.

## 🎯 Implemented Features

### 1. Directory Scanning 📁

**Feature**: Automatically load all supported image files from directories.

#### Capabilities:
- **Non-recursive scanning**: Scan only the top-level directory
- **Recursive scanning**: Scan directory and all subdirectories with `-R` flag
- **Smart filtering**: Automatically detects and loads only supported image formats
- **Alphabetical sorting**: Files are sorted consistently for predictable ordering
- **Mixed input**: Combine explicit files and directories in the same command

#### Supported Formats:
- **Static Images**: PNG, JPEG, WebP, TIFF/TIF, JPEG XL (JXL), BMP, TGA, PNM/PBM/PGM/PPM, Farbfeld
- **Animated**: GIF, MP4, WebM, Animated WebP

#### Usage Examples:
```bash
# Scan directory (non-recursive)
ww -S ~/wallpapers/

# Scan recursively
ww -S -R ~/wallpapers/

# Mix files and directories
ww -S image.jpg ~/wallpapers/ ~/more-images/

# Recursive scan with random order
ww -S -R -r ~/Pictures/
```

#### Implementation Details:
- **Location**: `src/ww.cc` - `ww_scan_directory()` function
- **API**: Simple C API for directory traversal
- **Memory management**: Proper cleanup with `ww_free_file_list()`
- **Error handling**: Validates directory existence and permissions
- **Cross-platform**: Uses POSIX directory APIs (dirent.h)

---

### 2. Smooth Transitions ✨

**Feature**: Animated transitions between wallpapers with multiple effect types.

#### Transition Types:

1. **Fade** (crossfade)
   - Smooth alpha blending between images
   - Default transition type
   - Configurable duration

2. **Slide Left**
   - Old image slides out to the left
   - New image slides in from the right

3. **Slide Right**
   - Old image slides out to the right
   - New image slides in from the left

4. **Slide Up**
   - Old image slides up
   - New image slides in from bottom

5. **Slide Down**
   - Old image slides down
   - New image slides in from top

6. **None**
   - Instant switch, no transition
   - Best for performance on slower systems

#### Features:
- **Configurable duration**: Set transition time from 0.1 to any number of seconds
- **Smooth easing**: Uses ease-in-out interpolation for natural motion
- **Frame-accurate**: Updates synchronized with display refresh rate
- **Memory efficient**: Double-buffering approach
- **Per-pixel blending**: High-quality transitions with proper alpha handling

#### Usage Examples:
```bash
# Default fade (1 second)
ww -S -t fade ~/wallpapers/

# Slow fade (3 seconds)
ww -S -t fade -d 3.0 ~/Pictures/*.jpg

# Quick slide transition
ww -S -t slide-left -d 0.5 ~/wallpapers/

# Slide from bottom with custom duration
ww -S -t slide-up -d 1.2 ~/Pictures/*.png

# No transition for instant switch
ww -S -t none ~/wallpapers/
```

#### Implementation Details:
- **Location**: `src/transition.cc` - Complete transition module
- **Architecture**: 
  - Separate transition state management
  - Modular transition effect functions
  - Frame-based animation system
- **Algorithms**:
  - Ease-in-out cubic interpolation for smooth motion
  - Linear interpolation for fade effects
  - Pixel-perfect sliding with proper boundary handling
- **Performance**: 
  - Optimized per-pixel operations
  - Minimal memory allocations during animation
  - Cache-friendly linear buffer access

---

## 🎬 Combined Usage

The real power comes from combining both features:

```bash
# Recursive directory scan with smooth fade transitions
ww -S -R -t fade -d 2.0 -i 300 ~/wallpapers/

# Random slideshow with slide transition
ww -S -R -r -t slide-left -d 1.5 -i 60 ~/Pictures/

# Quick gallery mode
ww -S -R -t fade -d 0.5 -i 10 ~/Portfolio/

# Ambient mode with long intervals and slow fade
ww -S -R -t fade -d 5.0 -i 3600 ~/Pictures/Ambient/
```

---

## 📊 Architecture

### Directory Scanning
```
User Command
    ↓
ww_scan_directory()
    ↓
scan_directory_recursive()
    ├── Read directory entries
    ├── Filter by extension
    ├── Recurse into subdirectories (if -R)
    └── Sort alphabetically
    ↓
Return file list
    ↓
Main slideshow loop
```

### Transitions
```
Slideshow Trigger
    ↓
Load new image
    ↓
ww_transition_create()
    ↓
ww_transition_start(old_buffer, new_buffer)
    ↓
Frame loop:
    ├── ww_transition_update(delta_time)
    ├── Apply transition effect
    │   ├── Fade: Alpha blending
    │   ├── Slide: Coordinate shifting
    │   └── None: Direct copy
    ├── Composite output buffer
    └── Render to surface
    ↓
ww_transition_destroy()
```

---

## 🔧 API Reference

### Directory Scanning API

```c
// Scan directory for images
int ww_scan_directory(const char *dir_path, 
                      ww_file_list_t *file_list, 
                      bool recursive);

// Free file list
void ww_free_file_list(ww_file_list_t *file_list);

// File list structure
typedef struct {
    char **paths;
    int count;
} ww_file_list_t;
```

### Transition API

```c
// Create transition state
ww_transition_state *ww_transition_create(
    ww_transition_type_t type,
    float duration,
    int width,
    int height
);

// Start transition
void ww_transition_start(
    ww_transition_state *state,
    const uint8_t *old_data,
    const uint8_t *new_data
);

// Update transition (call each frame)
bool ww_transition_update(
    ww_transition_state *state,
    float delta_time,
    uint8_t **output_data
);

// Clean up
void ww_transition_destroy(ww_transition_state *state);
```

---

## ✅ Testing

### Directory Scanning Tests
- ✓ Non-recursive scanning
- ✓ Recursive scanning with nested directories
- ✓ Mixed file and directory input
- ✓ Empty directory handling
- ✓ Permission error handling
- ✓ All supported format detection

### Transition Tests
- ✓ Fade transition with various durations
- ✓ All slide directions
- ✓ Easing curve smoothness
- ✓ Edge case handling (0 duration, very long duration)
- ✓ Memory leak testing
- ✓ Frame timing accuracy

---

## 🚀 Performance

### Directory Scanning
- **Speed**: Negligible overhead for most directory sizes
- **Memory**: O(n) where n = number of image files
- **Optimization**: Files loaded on-demand during slideshow

### Transitions
- **CPU Usage**: Moderate during transition, zero between transitions
- **Memory**: 3x image buffer size during transition
- **Frame Rate**: Maintains display refresh rate (typically 60 FPS)
- **Optimizations**:
  - SIMD-friendly linear memory layout
  - Minimal per-frame allocations
  - Cache-optimized buffer access patterns

---

## 📝 Configuration

### Command Line Options

```
-R, --recursive         Scan directories recursively
-t, --transition <type> Transition type: none, fade, slide-left, 
                        slide-right, slide-up, slide-down
-d, --duration <sec>    Transition duration in seconds (default: 1.0)
```

### Integration with Existing Options
- Works with `-S` (slideshow mode)
- Compatible with `-r` (random order)
- Supports `-i` (interval)
- Can be combined with all scaling modes (`-m`)
- Works with `-o` (specific output)

---

## 🔮 Future Enhancements

Potential improvements for these features:

### Directory Scanning
- [ ] Watch mode: Detect new files added to directory
- [ ] Exclude patterns/filters
- [ ] Metadata-based sorting (date, size, EXIF)
- [ ] Symbolic link handling options

### Transitions
- [ ] More transition types (zoom, rotate, wipe, dissolve)
- [ ] Custom transition curves
- [ ] GPU-accelerated transitions
- [ ] Per-image transition settings
- [ ] Transition presets (professional, creative, minimal)

---

## 📚 Resources

- See `EXAMPLES.md` for practical usage examples
- See `README.md` for installation instructions
- See `DEVELOPMENT.md` for development guidelines
- Run `ww --help` for quick reference

---

## 🤝 Contributing

Both features are modular and extensible. Contributions welcome for:
- New transition effects
- Performance optimizations
- Additional file format support
- Enhanced directory scanning options

---

**Implementation Date**: 2024
**Status**: ✅ Complete and tested
**Version**: 0.1.0+