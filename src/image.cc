#include "ww.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <webp/decode.h>
#include <tiffio.h>
#include <jxl/decode.h>
#include <jxl/decode_cxx.h>

extern "C" {

struct image_data_t {
    uint8_t *data;      // RGBA pixel data
    int width;
    int height;
    int channels;       // Original channels
};

// Load WebP image
static image_data_t* load_webp(const char *path) {
    if (!path) {
        return nullptr;
    }

    // Read file into memory
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open WebP file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *file_data = (uint8_t*)malloc(file_size);
    if (!file_data) {
        fclose(file);
        return nullptr;
    }

    if (fread(file_data, 1, file_size, file) != file_size) {
        fprintf(stderr, "Failed to read WebP file\n");
        free(file_data);
        fclose(file);
        return nullptr;
    }
    fclose(file);

    // Decode WebP
    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        free(file_data);
        return nullptr;
    }

    // Decode to RGBA
    img->data = WebPDecodeRGBA(file_data, file_size, &img->width, &img->height);
    img->channels = 4;

    free(file_data);

    if (!img->data) {
        fprintf(stderr, "Failed to decode WebP\n");
        free(img);
        return nullptr;
    }

    return img;
}

// Load TIFF image
static image_data_t* load_tiff(const char *path) {
    if (!path) {
        return nullptr;
    }

    TIFF *tif = TIFFOpen(path, "r");
    if (!tif) {
        fprintf(stderr, "Failed to open TIFF file: %s\n", path);
        return nullptr;
    }

    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        TIFFClose(tif);
        return nullptr;
    }

    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &img->width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &img->height);
    img->channels = 4;

    // Allocate RGBA buffer
    size_t npixels = img->width * img->height;
    img->data = (uint8_t*)malloc(npixels * 4);
    if (!img->data) {
        TIFFClose(tif);
        free(img);
        return nullptr;
    }

    // Read RGBA data
    if (!TIFFReadRGBAImageOriented(tif, img->width, img->height, 
                                   (uint32_t*)img->data, ORIENTATION_TOPLEFT, 0)) {
        fprintf(stderr, "Failed to read TIFF image\n");
        free(img->data);
        free(img);
        TIFFClose(tif);
        return nullptr;
    }

    TIFFClose(tif);

    // TIFFReadRGBAImage returns ABGR, convert to RGBA
    for (size_t i = 0; i < npixels; i++) {
        uint8_t r = img->data[i * 4 + 0];
        uint8_t g = img->data[i * 4 + 1];
        uint8_t b = img->data[i * 4 + 2];
        uint8_t a = img->data[i * 4 + 3];
        
        img->data[i * 4 + 0] = b;
        img->data[i * 4 + 1] = g;
        img->data[i * 4 + 2] = r;
        img->data[i * 4 + 3] = a;
    }

    return img;
}

// Load JXL (JPEG XL) image
static image_data_t* load_jxl(const char *path) {
    if (!path) {
        return nullptr;
    }

    // Read file into memory
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open JXL file: %s\n", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t *file_data = (uint8_t*)malloc(file_size);
    if (!file_data) {
        fclose(file);
        return nullptr;
    }

    if (fread(file_data, 1, file_size, file) != file_size) {
        fprintf(stderr, "Failed to read JXL file\n");
        free(file_data);
        fclose(file);
        return nullptr;
    }
    fclose(file);

    // Initialize decoder
    auto dec = JxlDecoderMake(nullptr);
    if (!dec) {
        fprintf(stderr, "Failed to create JXL decoder\n");
        free(file_data);
        return nullptr;
    }

    if (JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO | JXL_DEC_FULL_IMAGE) != JXL_DEC_SUCCESS) {
        fprintf(stderr, "Failed to subscribe to JXL events\n");
        free(file_data);
        return nullptr;
    }

    JxlDecoderSetInput(dec.get(), file_data, file_size);
    JxlDecoderCloseInput(dec.get());

    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        free(file_data);
        return nullptr;
    }
    img->data = nullptr;
    img->channels = 4;

    JxlBasicInfo info;
    JxlPixelFormat format = {4, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};

    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());

        if (status == JXL_DEC_ERROR) {
            fprintf(stderr, "JXL decoder error\n");
            free(file_data);
            free(img);
            return nullptr;
        } else if (status == JXL_DEC_NEED_MORE_INPUT) {
            fprintf(stderr, "JXL decoder needs more input\n");
            free(file_data);
            free(img);
            return nullptr;
        } else if (status == JXL_DEC_BASIC_INFO) {
            if (JxlDecoderGetBasicInfo(dec.get(), &info) != JXL_DEC_SUCCESS) {
                fprintf(stderr, "Failed to get JXL basic info\n");
                free(file_data);
                free(img);
                return nullptr;
            }
            img->width = info.xsize;
            img->height = info.ysize;
        } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            size_t buffer_size;
            if (JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size) != JXL_DEC_SUCCESS) {
                fprintf(stderr, "Failed to get JXL output buffer size\n");
                free(file_data);
                free(img);
                return nullptr;
            }

            img->data = (uint8_t*)malloc(buffer_size);
            if (!img->data) {
                free(file_data);
                free(img);
                return nullptr;
            }

            if (JxlDecoderSetImageOutBuffer(dec.get(), &format, img->data, buffer_size) != JXL_DEC_SUCCESS) {
                fprintf(stderr, "Failed to set JXL output buffer\n");
                free(img->data);
                free(file_data);
                free(img);
                return nullptr;
            }
        } else if (status == JXL_DEC_FULL_IMAGE) {
            // Successfully decoded
            break;
        } else if (status == JXL_DEC_SUCCESS) {
            break;
        }
    }

    free(file_data);

    if (!img->data) {
        fprintf(stderr, "Failed to decode JXL image\n");
        free(img);
        return nullptr;
    }

    return img;
}

// Load Farbfeld image (simple suckless format)
static image_data_t* load_farbfeld(const char *path) {
    if (!path) {
        return nullptr;
    }

    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open Farbfeld file: %s\n", path);
        return nullptr;
    }

    // Read header: "farbfeld" magic (8 bytes) + width (4) + height (4)
    uint8_t magic[8];
    if (fread(magic, 1, 8, file) != 8 || memcmp(magic, "farbfeld", 8) != 0) {
        fprintf(stderr, "Invalid Farbfeld magic\n");
        fclose(file);
        return nullptr;
    }

    uint32_t width_be, height_be;
    if (fread(&width_be, 4, 1, file) != 1 || fread(&height_be, 4, 1, file) != 1) {
        fprintf(stderr, "Failed to read Farbfeld dimensions\n");
        fclose(file);
        return nullptr;
    }

    // Convert from big-endian
    uint32_t width = __builtin_bswap32(width_be);
    uint32_t height = __builtin_bswap32(height_be);

    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        fclose(file);
        return nullptr;
    }

    img->width = width;
    img->height = height;
    img->channels = 4;
    img->data = (uint8_t*)malloc(width * height * 4);

    if (!img->data) {
        free(img);
        fclose(file);
        return nullptr;
    }

    // Read RGBA data (16-bit per channel, big-endian)
    for (uint32_t i = 0; i < width * height; i++) {
        uint16_t rgba[4];
        if (fread(rgba, 2, 4, file) != 4) {
            fprintf(stderr, "Failed to read Farbfeld pixel data\n");
            free(img->data);
            free(img);
            fclose(file);
            return nullptr;
        }

        // Convert 16-bit to 8-bit and from big-endian
        img->data[i * 4 + 0] = __builtin_bswap16(rgba[0]) >> 8; // R
        img->data[i * 4 + 1] = __builtin_bswap16(rgba[1]) >> 8; // G
        img->data[i * 4 + 2] = __builtin_bswap16(rgba[2]) >> 8; // B
        img->data[i * 4 + 3] = __builtin_bswap16(rgba[3]) >> 8; // A
    }

    fclose(file);
    return img;
}

// Load image from file using stb_image
static image_data_t* load_image(const char *path) {
    if (!path) {
        return nullptr;
    }

    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) {
        return nullptr;
    }

    // Force load as RGBA (4 channels)
    img->data = stbi_load(path, &img->width, &img->height, &img->channels, 4);
    
    if (!img->data) {
        fprintf(stderr, "stb_image error: %s\n", stbi_failure_reason());
        free(img);
        return nullptr;
    }

    return img;
}



// Scale image to fit output dimensions while preserving aspect ratio
static image_data_t* scale_image(const image_data_t *src, int target_width, int target_height, bool preserve_aspect) {
    if (!src || !src->data) {
        return nullptr;
    }

    int new_width = target_width;
    int new_height = target_height;

    if (preserve_aspect) {
        float src_aspect = (float)src->width / (float)src->height;
        float dst_aspect = (float)target_width / (float)target_height;

        if (src_aspect > dst_aspect) {
            // Image is wider than output
            new_width = target_width;
            new_height = (int)(target_width / src_aspect);
        } else {
            // Image is taller than output
            new_height = target_height;
            new_width = (int)(target_height * src_aspect);
        }
    }

    // Allocate new image
    image_data_t *scaled = (image_data_t*)malloc(sizeof(image_data_t));
    if (!scaled) {
        return nullptr;
    }

    scaled->width = new_width;
    scaled->height = new_height;
    scaled->channels = 4; // RGBA
    scaled->data = (uint8_t*)malloc(new_width * new_height * 4);
    
    if (!scaled->data) {
        free(scaled);
        return nullptr;
    }

    // Simple nearest-neighbor scaling
    // TODO: Use better scaling algorithm (bilinear/bicubic)
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = (x * src->width) / new_width;
            int src_y = (y * src->height) / new_height;
            
            int src_idx = (src_y * src->width + src_x) * 4;
            int dst_idx = (y * new_width + x) * 4;
            
            scaled->data[dst_idx + 0] = src->data[src_idx + 0]; // R
            scaled->data[dst_idx + 1] = src->data[src_idx + 1]; // G
            scaled->data[dst_idx + 2] = src->data[src_idx + 2]; // B
            scaled->data[dst_idx + 3] = src->data[src_idx + 3]; // A
        }
    }

    return scaled;
}

// Create a centered/letterboxed image on a black background
static image_data_t* center_image(const image_data_t *src, int canvas_width, int canvas_height) {
    if (!src || !src->data) {
        return nullptr;
    }

    image_data_t *canvas = (image_data_t*)malloc(sizeof(image_data_t));
    if (!canvas) {
        return nullptr;
    }

    canvas->width = canvas_width;
    canvas->height = canvas_height;
    canvas->channels = 4;
    canvas->data = (uint8_t*)malloc(canvas_width * canvas_height * 4);
    
    if (!canvas->data) {
        free(canvas);
        return nullptr;
    }
    
    // Fill with opaque black background (not transparent!)
    for (int i = 0; i < canvas_width * canvas_height; i++) {
        canvas->data[i * 4 + 0] = 0;   // R
        canvas->data[i * 4 + 1] = 0;   // G
        canvas->data[i * 4 + 2] = 0;   // B
        canvas->data[i * 4 + 3] = 255; // A (opaque!)
    }

    // Calculate centering offset
    int offset_x = (canvas_width - src->width) / 2;
    int offset_y = (canvas_height - src->height) / 2;

    // Copy image to center of canvas
    for (int y = 0; y < src->height && (y + offset_y) < canvas_height; y++) {
        if (y + offset_y < 0) continue;
        
        for (int x = 0; x < src->width && (x + offset_x) < canvas_width; x++) {
            if (x + offset_x < 0) continue;
            
            int src_idx = (y * src->width + x) * 4;
            int dst_idx = ((y + offset_y) * canvas_width + (x + offset_x)) * 4;
            
            canvas->data[dst_idx + 0] = src->data[src_idx + 0];
            canvas->data[dst_idx + 1] = src->data[src_idx + 1];
            canvas->data[dst_idx + 2] = src->data[src_idx + 2];
            canvas->data[dst_idx + 3] = src->data[src_idx + 3];
        }
    }

    return canvas;
}

// Public API for loading and processing images with scaling mode
image_data_t* ww_load_image_mode(const char *path, int output_width, int output_height, int mode, uint32_t bg_color) {
    // Check file type
    const char *ext = strrchr(path, '.');
    image_data_t *img = nullptr;
    
    if (ext) {
        ext++;
        if (strcasecmp(ext, "webp") == 0) {
            img = load_webp(path);
        } else if (strcasecmp(ext, "tiff") == 0 || strcasecmp(ext, "tif") == 0) {
            img = load_tiff(path);
        } else if (strcasecmp(ext, "jxl") == 0) {
            img = load_jxl(path);
        } else if (strcasecmp(ext, "ff") == 0) {
            img = load_farbfeld(path);
        }
    }
    
    // Fall back to stb_image for other formats
    if (!img) {
        img = load_image(path);
    }
    if (!img) {
        return nullptr;
    }

    // Handle different scaling modes
    image_data_t *result = nullptr;
    
    switch (mode) {
        case 0: // WW_MODE_FIT - scale to fit with letterboxing
        {
            if (img->width == output_width && img->height == output_height) {
                return img;
            }
            
            image_data_t *scaled = scale_image(img, output_width, output_height, true);
            if (img->data) free(img->data);
            free(img);
            
            if (!scaled) return nullptr;
            
            if (scaled->width != output_width || scaled->height != output_height) {
                result = center_image(scaled, output_width, output_height);
                if (scaled->data) free(scaled->data);
                free(scaled);
            } else {
                result = scaled;
            }
            break;
        }
        
        case 1: // WW_MODE_FILL - scale to fill, crop if needed
        {
            float img_aspect = (float)img->width / (float)img->height;
            float out_aspect = (float)output_width / (float)output_height;
            
            int scale_width, scale_height;
            if (img_aspect > out_aspect) {
                // Image is wider - scale by height
                scale_height = output_height;
                scale_width = (int)(output_height * img_aspect);
            } else {
                // Image is taller - scale by width
                scale_width = output_width;
                scale_height = (int)(output_width / img_aspect);
            }
            
            image_data_t *scaled = scale_image(img, scale_width, scale_height, false);
            if (img->data) free(img->data);
            free(img);
            
            if (!scaled) return nullptr;
            
            // Crop to output size if needed
            if (scaled->width != output_width || scaled->height != output_height) {
                result = center_image(scaled, output_width, output_height);
                if (scaled->data) free(scaled->data);
                free(scaled);
            } else {
                result = scaled;
            }
            break;
        }
        
        case 2: // WW_MODE_STRETCH - stretch to fill
        {
            result = scale_image(img, output_width, output_height, false);
            if (img->data) free(img->data);
            free(img);
            break;
        }
        
        case 3: // WW_MODE_CENTER - no scaling, just center
        {
            result = center_image(img, output_width, output_height);
            if (img->data) free(img->data);
            free(img);
            break;
        }
        
        case 4: // WW_MODE_TILE - repeat image to fill
        {
            result = (image_data_t*)malloc(sizeof(image_data_t));
            if (!result) {
                if (img->data) free(img->data);
                free(img);
                return nullptr;
            }
            
            result->width = output_width;
            result->height = output_height;
            result->channels = 4;
            result->data = (uint8_t*)malloc(output_width * output_height * 4);
            
            if (!result->data) {
                free(result);
                if (img->data) free(img->data);
                free(img);
                return nullptr;
            }
            
            // Tile the image
            for (int y = 0; y < output_height; y++) {
                for (int x = 0; x < output_width; x++) {
                    int src_x = x % img->width;
                    int src_y = y % img->height;
                    int src_idx = (src_y * img->width + src_x) * 4;
                    int dst_idx = (y * output_width + x) * 4;
                    
                    result->data[dst_idx + 0] = img->data[src_idx + 0];
                    result->data[dst_idx + 1] = img->data[src_idx + 1];
                    result->data[dst_idx + 2] = img->data[src_idx + 2];
                    result->data[dst_idx + 3] = img->data[src_idx + 3];
                }
            }
            
            if (img->data) free(img->data);
            free(img);
            break;
        }
        
        default:
            if (img->data) free(img->data);
            free(img);
            return nullptr;
    }

    return result;
}

// Legacy API for backward compatibility
image_data_t* ww_load_image(const char *path, int output_width, int output_height, bool preserve_aspect) {
    return ww_load_image_mode(path, output_width, output_height, preserve_aspect ? 0 : 2, 0x000000FF);
}

void ww_free_image(image_data_t *img) {
    if (!img) {
        return;
    }
    
    // For processed images (scaled/centered), data is malloc'd
    if (img->data) {
        free(img->data);
    }
    free(img);
}

} // extern "C"