# Development Guide

## Getting Started

### Prerequisites

- C++20 compatible compiler (GCC 10+, Clang 12+)
- xmake build system
- Wayland development libraries
- Image/video codec libraries

### Building

```bash
# Debug build (with symbols, no optimization)
xmake f -m debug
xmake

# Release build (optimized, stripped)
xmake f -m release
xmake

# Clean build
xmake clean
xmake
```

### Running

```bash
# Debug binary
./build/linux/x86_64/debug/ww --help

# Release binary
./build/linux/x86_64/release/ww --help

# Or use xmake run (note: arguments need careful quoting)
xmake run ww
```

## Project Architecture

### Directory Structure

```
ww/
├── inc/              # Public headers
│   └── ww.h         # Main API header
├── src/              # Implementation
│   ├── main.cc      # CLI entry point
│   ├── ww.cc        # Core API implementation
│   ├── wayland.cc   # Wayland protocol handling (TODO)
│   ├── image.cc     # Image decoding (TODO)
│   └── video.cc     # Video decoding (TODO)
├── xmake.lua        # Build configuration
└── README.md        # User documentation
```

### Core Components

1. **CLI Layer** (`main.cc`)
   - Argument parsing
   - User-facing interface
   - Calls into core API

2. **Core API** (`ww.cc`)
   - Public C API for library usage
   - Error handling
   - File type detection

3. **Wayland Layer** (TODO: `wayland.cc`)
   - Display connection
   - Output enumeration
   - Layer shell protocol
   - Surface management

4. **Image Decoder** (TODO: `image.cc`)
   - PNG decoding (libpng)
   - JPEG decoding (libjpeg-turbo)
   - WebP decoding (libwebp)
   - Scaling and format conversion

5. **Video Decoder** (TODO: `video.cc`)
   - FFmpeg integration
   - Frame extraction
   - GIF animation
   - Video