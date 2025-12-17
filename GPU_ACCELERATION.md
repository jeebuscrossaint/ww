# GPU Acceleration Plan for ww

## Overview

This document outlines the plan for implementing GPU-accelerated rendering in `ww` to improve performance, especially for transitions and high-resolution displays.

## Current Architecture

### Software Rendering (Current Implementation)

```
Image Loading → CPU Scaling → CPU Transitions → CPU Format Conversion → Wayland SHM Buffer
     ↓              ↓              ↓                    ↓                      ↓
   libpng      Bicubic/Bilinear  Pixel blending     RGBA→BGRA            wl_shm
   libjpeg     (CPU intensive)   (CPU intensive)   (memcpy heavy)     (shared memory)
   FFmpeg
```

**Performance Characteristics:**
- CPU-bound for all operations
- Memory bandwidth limited
- 4K transitions: ~40-50% CPU usage on modern CPUs
- Scales poorly with resolution (4K is 4x more expensive than 1080p)

**Current Transition Performance:**
- 1920x1080 @ 60 FPS: ~5-10% CPU
- 2560x1600 @ 60 FPS: ~10-15% CPU
- 3840x2160 @ 60 FPS: ~40-50% CPU

## GPU Acceleration Benefits

### Performance Improvements
- **10-100x faster transitions** (GPU parallel processing)
- **Lower CPU usage** (offload to GPU)
- **Better battery life** on laptops
- **Hardware video decoding** (VAAPI/VDPAU)
- **Real-time effects** (blur, filters) without CPU overhead

### Quality Improvements
- **Hardware-accelerated scaling** (better than bicubic)
- **Anti-aliased transitions**
- **Complex effects** (3D transforms, shaders)
- **Multiple simultaneous transitions** (per-monitor)

## Implementation Approaches

### Option 1: Vulkan (Recommended)

**Pros:**
- Cross-platform (Linux, BSD, etc.)
- Modern API with explicit control
- Excellent performance
- Growing ecosystem
- Well-documented

**Cons:**
- More complex than OpenGL
- Requires more boilerplate
- Steeper learning curve

**Libraries:**
- `vulkan` - Core Vulkan API
- `vk-bootstrap` - Simplify Vulkan setup
- `VMA` (Vulkan Memory Allocator) - Memory management

**Architecture:**
```
Wayland Display
    ↓
wl_surface (DMA-BUF or Pixmap)
    ↓
Vulkan Swapchain
    ↓
Render Pass → Compute Shaders → Fragment Shaders
    ↓              ↓                  ↓
Transitions    Scaling/Filters    Final Composite
```

### Option 2: OpenGL ES / EGL

**Pros:**
- Simpler API than Vulkan
- Well-established
- Good documentation
- Easier to prototype

**Cons:**
- Older API design
- Less explicit control
- Potentially higher driver overhead
- OpenGL ES more limited than desktop GL

**Libraries:**
- `EGL` - Platform integration (Wayland)
- `OpenGL ES 3.x` - Rendering API
- `GLFW` or direct EGL/Wayland integration

### Option 3: Hybrid Approach

**CPU for image loading, GPU for rendering:**
```
CPU: Image decode (libpng, libjpeg, FFmpeg)
  ↓
GPU Texture Upload
  ↓
GPU: Scaling, Transitions, Effects
  ↓
Wayland DMA-BUF (zero-copy)
```

## Recommended Implementation Path

### Phase 1: Foundation (Estimated: 1-2 days)

1. **Add Vulkan/EGL initialization**
   - Integrate with existing Wayland code
   - Create rendering context
   - Set up swapchain/framebuffer

2. **Basic texture rendering**
   - Upload decoded images to GPU
   - Simple fullscreen quad rendering
   - Test on single output

3. **Replace CPU format conversion**
   - RGBA→BGRA in shader instead of CPU
   - Immediate performance win

**Files to modify:**
- `src/wayland.cc` - Add GPU context
- `src/gpu.cc` (new) - GPU abstraction layer
- `xmake.lua` - Add Vulkan/EGL dependencies

### Phase 2: GPU Transitions (Estimated: 2-3 days)

1. **Port existing transitions to shaders**
   - Fade → Fragment shader with alpha blend
   - Slide → Texture coordinate animation
   - Zoom → Scale matrix transformation

2. **Shader-based effects**
   - Circle wipe → Distance field shader
   - Dissolve → Noise function
   - Pixelate → Compute shader or fragment shader

3. **Performance comparison**
   - Benchmark CPU vs GPU
   - Optimize shader code

**New files:**
- `src/shaders/fade.frag`
- `src/shaders/slide.frag`
- `src/shaders/zoom.frag`
- `src/shaders/circle.frag`
- etc.

### Phase 3: Advanced Features (Estimated: 3-5 days)

1. **Hardware video decode**
   - VAAPI integration (Intel/AMD)
   - VDPAU integration (NVIDIA)
   - Direct GPU texture rendering

2. **GPU scaling**
   - Replace CPU bicubic with GPU Lanczos
   - Hardware acceleration

3. **Real-time filters**
   - Blur (Gaussian)
   - Brightness/Contrast/Saturation
   - Color adjustments

4. **Multi-output optimization**
   - Render to multiple surfaces in parallel
   - Share textures between outputs

### Phase 4: Optimization (Estimated: 1-2 days)

1. **Zero-copy paths**
   - DMA-BUF for Wayland
   - Avoid CPU↔GPU transfers

2. **Memory management**
   - Efficient texture pooling
   - Reduce allocations

3. **Performance profiling**
   - GPU timers
   - Bottleneck identification

## Code Structure (Proposed)

```
ww/
├── src/
│   ├── gpu/
│   │   ├── gpu.h           # GPU abstraction API
│   │   ├── vulkan.cc       # Vulkan implementation
│   │   ├── gles.cc         # OpenGL ES implementation (fallback)
│   │   └── cpu.cc          # CPU fallback (current implementation)
│   ├── shaders/
│   │   ├── common.h        # Shared shader code
│   │   ├── fade.frag       # Fade transition shader
│   │   ├── slide.frag      # Slide transition shader
│   │   ├── zoom.frag       # Zoom transition shader
│   │   ├── circle.frag     # Circle wipe shader
│   │   ├── dissolve.frag   # Dissolve shader
│   │   └── scale.comp      # Scaling compute shader
│   └── ... (existing files)
└── ...
```

## API Design (Proposed)

```cpp
// GPU abstraction layer
class GPURenderer {
public:
    virtual ~GPURenderer() = default;
    
    // Initialization
    virtual bool init(wl_display *display) = 0;
    virtual void cleanup() = 0;
    
    // Texture management
    virtual GPUTexture* upload_image(const image_data_t *img) = 0;
    virtual void free_texture(GPUTexture *tex) = 0;
    
    // Rendering
    virtual void begin_frame() = 0;
    virtual void render_transition(
        GPUTexture *old_tex,
        GPUTexture *new_tex,
        ww_transition_type_t type,
        float progress
    ) = 0;
    virtual void end_frame() = 0;
    
    // Output management
    virtual void set_output_size(int width, int height) = 0;
    virtual wl_buffer* get_buffer() = 0;
};

// Factory function
GPURenderer* create_gpu_renderer(GPUBackend backend);

enum class GPUBackend {
    AUTO,      // Auto-detect best available
    VULKAN,    // Vulkan
    GLES,      // OpenGL ES
    CPU,       // CPU fallback (current implementation)
};
```

## CLI Integration

```bash
# GPU acceleration options
ww --gpu vulkan ...          # Force Vulkan backend
ww --gpu gles ...            # Force OpenGL ES backend
ww --gpu cpu ...             # Force CPU rendering (current)
ww --gpu auto ...            # Auto-detect (default)

# Query GPU info
ww --gpu-info                # Show available GPU backends
ww --benchmark               # Benchmark CPU vs GPU performance
```

## Shader Examples

### Fade Transition (GLSL)

```glsl
#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D oldImage;
layout(binding = 1) uniform sampler2D newImage;

layout(push_constant) uniform PushConstants {
    float progress;
} pc;

void main() {
    vec4 oldColor = texture(oldImage, fragTexCoord);
    vec4 newColor = texture(newImage, fragTexCoord);
    outColor = mix(oldColor, newColor, pc.progress);
}
```

### Circle Wipe Transition (GLSL)

```glsl
#version 450

layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D oldImage;
layout(binding = 1) uniform sampler2D newImage;

layout(push_constant) uniform PushConstants {
    float progress;
    vec2 resolution;
} pc;

void main() {
    vec2 center = vec2(0.5, 0.5);
    vec2 uv = fragTexCoord;
    
    float maxDist = length(pc.resolution) / 2.0;
    float radius = pc.progress * maxDist;
    float dist = length((uv - center) * pc.resolution);
    
    vec4 oldColor = texture(oldImage, uv);
    vec4 newColor = texture(newImage, uv);
    
    outColor = (dist < radius) ? newColor : oldColor;
}
```

## Performance Targets

### With GPU Acceleration

| Resolution  | CPU Usage (60 FPS) | GPU Usage | Memory Bandwidth |
|-------------|-------------------|-----------|------------------|
| 1920x1080   | <2%              | ~5%       | Minimal          |
| 2560x1600   | <3%              | ~8%       | Minimal          |
| 3840x2160   | <5%              | ~15%      | Minimal          |
| 5120x2880   | <8%              | ~25%      | Low              |

## Testing Plan

1. **Unit tests**
   - GPU initialization
   - Texture upload/download
   - Shader compilation

2. **Integration tests**
   - Transition rendering
   - Multi-output
   - Fallback to CPU

3. **Performance tests**
   - Benchmark all transitions
   - Compare CPU vs GPU
   - Memory usage profiling

4. **Compatibility tests**
   - Different GPUs (Intel, AMD, NVIDIA)
   - Different compositors (Sway, Hyprland, River)
   - Fallback behavior

## Dependencies

### Required Packages

```bash
# Vulkan
vulkan-headers
vulkan-loader
vulkan-validation-layers (development)

# OR OpenGL ES
libgles2
libegl1

# Optional (hardware decode)
libva-dev        # VAAPI
libvdpau-dev     # VDPAU
```

### xmake Configuration

```lua
add_requires("vulkan-headers", "vulkan-loader")
-- OR
add_requires("gles2", "egl")

-- Optional
add_requires("libva", {optional = true})
add_requires("vdpau", {optional = true})
```

## Fallback Strategy

```
Try Vulkan
  ↓ (if fails)
Try OpenGL ES
  ↓ (if fails)
Fall back to CPU rendering (current implementation)
```

**Fallback triggers:**
- GPU not available
- Driver errors
- Out of memory
- Compositor incompatibility

## Risks and Mitigations

| Risk | Impact | Mitigation |
|------|--------|------------|
| GPU driver bugs | High | Robust fallback to CPU |
| Increased complexity | Medium | Good abstraction layer |
| DMA-BUF compatibility | Medium | Test on multiple compositors |
| Memory leaks | High | Careful resource management |
| Shader compilation time | Low | Pre-compile and cache shaders |

## Timeline Estimate

- **Phase 1** (Foundation): 1-2 days
- **Phase 2** (Transitions): 2-3 days
- **Phase 3** (Advanced): 3-5 days
- **Phase 4** (Optimization): 1-2 days
- **Testing/Debugging**: 2-3 days

**Total: 9-15 days** for full GPU acceleration implementation

## Success Criteria

1. ✅ GPU rendering works on major GPUs (Intel, AMD, NVIDIA)
2. ✅ Performance improvement of 5-10x for 4K transitions
3. ✅ CPU usage <5% during 4K/60 FPS transitions
4. ✅ Graceful fallback to CPU if GPU unavailable
5. ✅ No visual regressions
6. ✅ Stable across different compositors

## Next Steps

1. **Prototype Phase 1** with Vulkan
2. **Measure performance gains**
3. **Evaluate complexity vs benefit**
4. **Decide on implementation approach**

---

**Status**: Planning phase
**Last Updated**: 2024
**Priority**: Medium (current CPU implementation is performant enough for most use cases)

## References

- [Vulkan Tutorial](https://vulkan-tutorial.com/)
- [Wayland Book - DMA-BUF](https://wayland-book.com/)
- [EGL/Wayland Integration](https://wayland.freedesktop.org/egl.html)
- [GPU Gems - Real-Time Rendering](https://developer.nvidia.com/gpugems/gpugems/contributors)