-- Project metadata
set_project("ww")
set_version("0.1.0")
set_languages("c++20")

-- Build modes
add_rules("mode.debug", "mode.release")

if is_mode("release") then
    set_optimize("fastest")
    set_strip("all")
    add_vectorexts("sse2", "sse3", "ssse3", "sse4.2", "avx")
end

if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
    add_defines("DEBUG")
end

-- Target: ww (wallpaper setter)
target("ww")
    set_kind("binary")

    -- Source and header directories
    add_includedirs("inc")
    add_includedirs("build/protocols")
    add_files("src/*.cc")
    add_files("src/*.c")

    -- Wayland dependencies
    add_packages("wayland", "wayland-protocols")

    -- Image format support (stb handles PNG, JPEG, BMP, TGA, GIF, PNM)
    add_packages("stb", "libwebp", "libtiff", "libjxl")

    -- Video/Animation support (FFmpeg for GIF, WebP, MP4, WebM)
    add_packages("ffmpeg")

    -- Additional useful libs
    add_syslinks("pthread", "m", "wayland-client", "avcodec", "avformat", "avutil", "swscale")

    -- Link the protocol object files (compile as C, not C++)
    add_files("build/protocols/wlr-layer-shell-unstable-v1-protocol.c", {languages = "c"})
    add_files("build/protocols/xdg-shell-protocol.c", {languages = "c"})

    -- Compiler flags
    add_cxxflags("-Wall", "-Wextra", "-Wpedantic")
    add_cxxflags("-fno-exceptions", "-fno-rtti", {force = true}) -- Optional: for max performance

    -- Link time optimization in release
    if is_mode("release") then
        add_ldflags("-flto")
        add_cxxflags("-flto")
    end
target_end()

-- Package requirements (xmake will try to find/install these)
add_requires("wayland")
add_requires("wayland-protocols")
add_requires("stb")
add_requires("libwebp")
add_requires("libtiff")
add_requires("libjxl")
add_requires("ffmpeg")
