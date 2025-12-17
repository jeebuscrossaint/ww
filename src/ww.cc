#include "ww.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>

// Error handling
static thread_local char error_buffer[256] = {0};

void set_error(const char *msg) {
    snprintf(error_buffer, sizeof(error_buffer), "%s", msg);
}

const char *ww_get_error(void) {
    return error_buffer;
}

// File type detection
ww_filetype_t ww_detect_filetype(const char *path) {
    if (!path) {
        set_error("NULL path provided");
        return WW_TYPE_UNKNOWN;
    }

    // Check file exists
    struct stat st;
    if (stat(path, &st) != 0) {
        set_error("File does not exist");
        return WW_TYPE_UNKNOWN;
    }

    // Simple extension-based detection
    const char *ext = strrchr(path, '.');
    if (!ext) {
        set_error("No file extension found");
        return WW_TYPE_UNKNOWN;
    }

    ext++; // Skip the dot

    // Case-insensitive comparison
    if (strcasecmp(ext, "png") == 0) {
        return WW_TYPE_PNG;
    } else if (strcasecmp(ext, "jpg") == 0 || strcasecmp(ext, "jpeg") == 0) {
        return WW_TYPE_JPEG;
    } else if (strcasecmp(ext, "webp") == 0) {
        return WW_TYPE_WEBP;
    } else if (strcasecmp(ext, "bmp") == 0) {
        return WW_TYPE_BMP;
    } else if (strcasecmp(ext, "tga") == 0) {
        return WW_TYPE_TGA;
    } else if (strcasecmp(ext, "pnm") == 0 || strcasecmp(ext, "pbm") == 0 || 
               strcasecmp(ext, "pgm") == 0 || strcasecmp(ext, "ppm") == 0) {
        return WW_TYPE_PNM;
    } else if (strcasecmp(ext, "tiff") == 0 || strcasecmp(ext, "tif") == 0) {
        return WW_TYPE_TIFF;
    } else if (strcasecmp(ext, "jxl") == 0) {
        return WW_TYPE_JXL;
    } else if (strcasecmp(ext, "ff") == 0) {
        return WW_TYPE_FARBFELD;
    } else if (strcasecmp(ext, "gif") == 0) {
        return WW_TYPE_GIF;
    } else if (strcasecmp(ext, "mp4") == 0) {
        return WW_TYPE_MP4;
    } else if (strcasecmp(ext, "webm") == 0) {
        return WW_TYPE_WEBM;
    }

    set_error("Unsupported file extension");
    return WW_TYPE_UNKNOWN;
}

// Wayland initialization, output listing, and wallpaper setting
// are now implemented in wayland.cc