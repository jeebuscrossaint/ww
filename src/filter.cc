#include "ww.h"
#include <cmath>
#include <cstring>
#include <algorithm>

extern "C" {

// Helper to get image data structure (defined in image.cc)
struct image_data_t {
    uint8_t *data;
    int width;
    int height;
    int channels;
};

// Clamp value between 0 and 255
static inline uint8_t clamp_u8(float value) {
    if (value < 0.0f) return 0;
    if (value > 255.0f) return 255;
    return (uint8_t)value;
}

// Apply brightness adjustment
void ww_apply_brightness(image_data_t *img, float brightness) {
    if (!img || !img->data || brightness == 1.0f) return;
    
    size_t pixel_count = img->width * img->height * 4;
    
    for (size_t i = 0; i < pixel_count; i += 4) {
        img->data[i + 0] = clamp_u8(img->data[i + 0] * brightness);
        img->data[i + 1] = clamp_u8(img->data[i + 1] * brightness);
        img->data[i + 2] = clamp_u8(img->data[i + 2] * brightness);
        // Alpha unchanged
    }
}

// Apply contrast adjustment
void ww_apply_contrast(image_data_t *img, float contrast) {
    if (!img || !img->data || contrast == 1.0f) return;
    
    float factor = (259.0f * (contrast * 255.0f + 255.0f)) / 
                   (255.0f * (259.0f - contrast * 255.0f));
    
    size_t pixel_count = img->width * img->height * 4;
    
    for (size_t i = 0; i < pixel_count; i += 4) {
        for (int c = 0; c < 3; c++) {
            float color = img->data[i + c];
            color = factor * (color - 128.0f) + 128.0f;
            img->data[i + c] = clamp_u8(color);
        }
        // Alpha unchanged
    }
}

// Apply saturation adjustment
void ww_apply_saturation(image_data_t *img, float saturation) {
    if (!img || !img->data || saturation == 1.0f) return;
    
    size_t pixel_count = img->width * img->height * 4;
    
    for (size_t i = 0; i < pixel_count; i += 4) {
        uint8_t r = img->data[i + 0];
        uint8_t g = img->data[i + 1];
        uint8_t b = img->data[i + 2];
        
        // Calculate grayscale value (luminance)
        float gray = 0.299f * r + 0.587f * g + 0.114f * b;
        
        // Interpolate between grayscale and original color
        img->data[i + 0] = clamp_u8(gray + saturation * (r - gray));
        img->data[i + 1] = clamp_u8(gray + saturation * (g - gray));
        img->data[i + 2] = clamp_u8(gray + saturation * (b - gray));
        // Alpha unchanged
    }
}

// Apply box blur (simple but fast)
void ww_apply_blur(image_data_t *img, float radius) {
    if (!img || !img->data || radius <= 0.0f) return;
    
    int r = (int)radius;
    if (r < 1) r = 1;
    
    // Allocate temporary buffer
    uint8_t *temp = (uint8_t*)malloc(img->width * img->height * 4);
    if (!temp) return;
    
    // Horizontal pass
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;
            int count = 0;
            
            for (int dx = -r; dx <= r; dx++) {
                int nx = x + dx;
                if (nx >= 0 && nx < img->width) {
                    int idx = (y * img->width + nx) * 4;
                    sum_r += img->data[idx + 0];
                    sum_g += img->data[idx + 1];
                    sum_b += img->data[idx + 2];
                    sum_a += img->data[idx + 3];
                    count++;
                }
            }
            
            int idx = (y * img->width + x) * 4;
            temp[idx + 0] = sum_r / count;
            temp[idx + 1] = sum_g / count;
            temp[idx + 2] = sum_b / count;
            temp[idx + 3] = sum_a / count;
        }
    }
    
    // Vertical pass (back to original buffer)
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            int sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;
            int count = 0;
            
            for (int dy = -r; dy <= r; dy++) {
                int ny = y + dy;
                if (ny >= 0 && ny < img->height) {
                    int idx = (ny * img->width + x) * 4;
                    sum_r += temp[idx + 0];
                    sum_g += temp[idx + 1];
                    sum_b += temp[idx + 2];
                    sum_a += temp[idx + 3];
                    count++;
                }
            }
            
            int idx = (y * img->width + x) * 4;
            img->data[idx + 0] = sum_r / count;
            img->data[idx + 1] = sum_g / count;
            img->data[idx + 2] = sum_b / count;
            img->data[idx + 3] = sum_a / count;
        }
    }
    
    free(temp);
}

// Apply all filters in a filter struct
void ww_apply_filter(image_data_t *img, const ww_filter_t *filter) {
    if (!img || !filter) return;
    
    // Apply filters in order
    if (filter->brightness != 1.0f) {
        ww_apply_brightness(img, filter->brightness);
    }
    
    if (filter->contrast != 1.0f) {
        ww_apply_contrast(img, filter->contrast);
    }
    
    if (filter->saturation != 1.0f) {
        ww_apply_saturation(img, filter->saturation);
    }
    
    if (filter->blur > 0.0f) {
        ww_apply_blur(img, filter->blur);
    }
}

// Blend two images with alpha
image_data_t *ww_blend_images(const image_data_t *img1, const image_data_t *img2, float alpha) {
    if (!img1 || !img2) return nullptr;
    if (img1->width != img2->width || img1->height != img2->height) return nullptr;
    
    // Clamp alpha
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    
    // Create result image
    image_data_t *result = (image_data_t*)malloc(sizeof(image_data_t));
    if (!result) return nullptr;
    
    result->width = img1->width;
    result->height = img1->height;
    result->channels = 4;
    result->data = (uint8_t*)malloc(result->width * result->height * 4);
    
    if (!result->data) {
        free(result);
        return nullptr;
    }
    
    // Blend pixels
    size_t pixel_count = result->width * result->height * 4;
    for (size_t i = 0; i < pixel_count; i++) {
        float val1 = img1->data[i];
        float val2 = img2->data[i];
        result->data[i] = clamp_u8(val1 * (1.0f - alpha) + val2 * alpha);
    }
    
    return result;
}

// Transition between two images
image_data_t *ww_transition_images(const image_data_t *from, const image_data_t *to,
                                    ww_transition_type_t type, float progress) {
    if (!from || !to) return nullptr;
    if (from->width != to->width || from->height != to->height) return nullptr;
    
    // Clamp progress
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;
    
    switch (type) {
        case WW_TRANSITION_FADE:
            return ww_blend_images(from, to, progress);
            
        case WW_TRANSITION_SLIDE_LEFT:
        case WW_TRANSITION_SLIDE_RIGHT:
        case WW_TRANSITION_SLIDE_UP:
        case WW_TRANSITION_SLIDE_DOWN: {
            // Create result image
            image_data_t *result = (image_data_t*)malloc(sizeof(image_data_t));
            if (!result) return nullptr;
            
            result->width = from->width;
            result->height = from->height;
            result->channels = 4;
            result->data = (uint8_t*)malloc(result->width * result->height * 4);
            
            if (!result->data) {
                free(result);
                return nullptr;
            }
            
            // Calculate offset based on transition type and progress
            int offset_x = 0, offset_y = 0;
            
            if (type == WW_TRANSITION_SLIDE_LEFT) {
                offset_x = -(int)(progress * result->width);
            } else if (type == WW_TRANSITION_SLIDE_RIGHT) {
                offset_x = (int)(progress * result->width);
            } else if (type == WW_TRANSITION_SLIDE_UP) {
                offset_y = -(int)(progress * result->height);
            } else if (type == WW_TRANSITION_SLIDE_DOWN) {
                offset_y = (int)(progress * result->height);
            }
            
            // Copy pixels with sliding effect
            for (int y = 0; y < result->height; y++) {
                for (int x = 0; x < result->width; x++) {
                    int src_x = x - offset_x;
                    int src_y = y - offset_y;
                    
                    const uint8_t *src_pixel;
                    
                    // Determine which image to sample from
                    if (src_x >= 0 && src_x < result->width && 
                        src_y >= 0 && src_y < result->height) {
                        // Sample from "from" image
                        src_pixel = &from->data[(src_y * from->width + src_x) * 4];
                    } else {
                        // Sample from "to" image (wrapped around)
                        int to_x = (src_x + result->width) % result->width;
                        int to_y = (src_y + result->height) % result->height;
                        if (to_x < 0) to_x += result->width;
                        if (to_y < 0) to_y += result->height;
                        src_pixel = &to->data[(to_y * to->width + to_x) * 4];
                    }
                    
                    int dst_idx = (y * result->width + x) * 4;
                    result->data[dst_idx + 0] = src_pixel[0];
                    result->data[dst_idx + 1] = src_pixel[1];
                    result->data[dst_idx + 2] = src_pixel[2];
                    result->data[dst_idx + 3] = src_pixel[3];
                }
            }
            
            return result;
        }
            
        case WW_TRANSITION_NONE:
        default:
            // Just return a copy of the "to" image
            return ww_blend_images(to, to, 1.0f);
    }
}

} // extern "C"