#include "ww.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

extern "C" {

// Helper to get image data structure
struct image_data_t {
    uint8_t *data;
    int width;
    int height;
    int channels;
};

// Linear interpolation
static inline float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// Clamp value between 0 and 1
static inline float clamp01(float value) {
    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;
}

// Interpolate between gradient stops
static uint32_t interpolate_gradient(const ww_gradient_config_t *config, float t) {
    if (!config || !config->stops || config->stop_count < 1) {
        return 0xFF000000; // Black
    }
    
    t = clamp01(t);
    
    // If only one stop, return that color
    if (config->stop_count == 1) {
        return config->stops[0].color;
    }
    
    // Find the two stops to interpolate between
    int idx1 = 0, idx2 = 0;
    
    for (int i = 0; i < config->stop_count - 1; i++) {
        if (t >= config->stops[i].position && t <= config->stops[i + 1].position) {
            idx1 = i;
            idx2 = i + 1;
            break;
        }
    }
    
    // Handle edge cases
    if (t <= config->stops[0].position) {
        return config->stops[0].color;
    }
    if (t >= config->stops[config->stop_count - 1].position) {
        return config->stops[config->stop_count - 1].color;
    }
    
    // Interpolate between the two colors
    float pos1 = config->stops[idx1].position;
    float pos2 = config->stops[idx2].position;
    float local_t = (t - pos1) / (pos2 - pos1);
    
    uint32_t c1 = config->stops[idx1].color;
    uint32_t c2 = config->stops[idx2].color;
    
    // Extract RGBA components
    uint8_t r1 = (c1 >> 24) & 0xFF;
    uint8_t g1 = (c1 >> 16) & 0xFF;
    uint8_t b1 = (c1 >> 8) & 0xFF;
    uint8_t a1 = c1 & 0xFF;
    
    uint8_t r2 = (c2 >> 24) & 0xFF;
    uint8_t g2 = (c2 >> 16) & 0xFF;
    uint8_t b2 = (c2 >> 8) & 0xFF;
    uint8_t a2 = c2 & 0xFF;
    
    // Interpolate each component
    uint8_t r = (uint8_t)lerp(r1, r2, local_t);
    uint8_t g = (uint8_t)lerp(g1, g2, local_t);
    uint8_t b = (uint8_t)lerp(b1, b2, local_t);
    uint8_t a = (uint8_t)lerp(a1, a2, local_t);
    
    return (r << 24) | (g << 16) | (b << 8) | a;
}

// Generate linear gradient
static void generate_linear_gradient(image_data_t *img, const ww_gradient_config_t *config) {
    float angle_rad = config->angle * M_PI / 180.0f;
    float cos_a = cosf(angle_rad);
    float sin_a = sinf(angle_rad);
    
    // Calculate the gradient line endpoints
    float dx = cos_a;
    float dy = sin_a;
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Normalize coordinates to -1..1
            float nx = (2.0f * x / img->width) - 1.0f;
            float ny = (2.0f * y / img->height) - 1.0f;
            
            // Project point onto gradient line
            float t = (nx * dx + ny * dy) * 0.5f + 0.5f;
            
            // Get color from gradient
            uint32_t color = interpolate_gradient(config, t);
            
            // Extract RGBA
            uint8_t r = (color >> 24) & 0xFF;
            uint8_t g = (color >> 16) & 0xFF;
            uint8_t b = (color >> 8) & 0xFF;
            uint8_t a = color & 0xFF;
            
            // Write to image
            int idx = (y * img->width + x) * 4;
            img->data[idx + 0] = r;
            img->data[idx + 1] = g;
            img->data[idx + 2] = b;
            img->data[idx + 3] = a;
        }
    }
}

// Generate radial gradient
static void generate_radial_gradient(image_data_t *img, const ww_gradient_config_t *config) {
    float center_x = config->center_x * img->width;
    float center_y = config->center_y * img->height;
    
    // Calculate max distance from center to corner
    float max_dist = 0.0f;
    float corners[4][2] = {
        {0.0f, 0.0f},
        {(float)img->width, 0.0f},
        {0.0f, (float)img->height},
        {(float)img->width, (float)img->height}
    };
    
    for (int i = 0; i < 4; i++) {
        float dx = corners[i][0] - center_x;
        float dy = corners[i][1] - center_y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist > max_dist) {
            max_dist = dist;
        }
    }
    
    if (max_dist == 0.0f) max_dist = 1.0f;
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Calculate distance from center
            float dx = x - center_x;
            float dy = y - center_y;
            float dist = sqrtf(dx * dx + dy * dy);
            
            // Normalize to 0..1
            float t = dist / max_dist;
            
            // Get color from gradient
            uint32_t color = interpolate_gradient(config, t);
            
            // Extract RGBA
            uint8_t r = (color >> 24) & 0xFF;
            uint8_t g = (color >> 16) & 0xFF;
            uint8_t b = (color >> 8) & 0xFF;
            uint8_t a = color & 0xFF;
            
            // Write to image
            int idx = (y * img->width + x) * 4;
            img->data[idx + 0] = r;
            img->data[idx + 1] = g;
            img->data[idx + 2] = b;
            img->data[idx + 3] = a;
        }
    }
}

// Generate angular/conic gradient
static void generate_angular_gradient(image_data_t *img, const ww_gradient_config_t *config) {
    float center_x = config->center_x * img->width;
    float center_y = config->center_y * img->height;
    float angle_offset = config->angle * M_PI / 180.0f;
    
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            // Calculate angle from center
            float dx = x - center_x;
            float dy = y - center_y;
            float angle = atan2f(dy, dx);
            
            // Apply offset and normalize to 0..1
            angle += angle_offset;
            float t = (angle + M_PI) / (2.0f * M_PI);
            
            // Wrap around
            t = fmodf(t, 1.0f);
            if (t < 0.0f) t += 1.0f;
            
            // Get color from gradient
            uint32_t color = interpolate_gradient(config, t);
            
            // Extract RGBA
            uint8_t r = (color >> 24) & 0xFF;
            uint8_t g = (color >> 16) & 0xFF;
            uint8_t b = (color >> 8) & 0xFF;
            uint8_t a = color & 0xFF;
            
            // Write to image
            int idx = (y * img->width + x) * 4;
            img->data[idx + 0] = r;
            img->data[idx + 1] = g;
            img->data[idx + 2] = b;
            img->data[idx + 3] = a;
        }
    }
}

// Main gradient generation function
image_data_t *ww_generate_gradient(int width, int height, const ww_gradient_config_t *config) {
    if (width <= 0 || height <= 0 || !config) {
        return nullptr;
    }
    
    // Allocate image structure
    image_data_t *img = (image_data_t*)malloc(sizeof(image_data_t));
    if (!img) return nullptr;
    
    img->width = width;
    img->height = height;
    img->channels = 4;
    img->data = (uint8_t*)malloc(width * height * 4);
    
    if (!img->data) {
        free(img);
        return nullptr;
    }
    
    // Generate gradient based on type
    switch (config->type) {
        case WW_GRADIENT_LINEAR:
            generate_linear_gradient(img, config);
            break;
            
        case WW_GRADIENT_RADIAL:
            generate_radial_gradient(img, config);
            break;
            
        case WW_GRADIENT_ANGULAR:
            generate_angular_gradient(img, config);
            break;
            
        default:
            // Fill with black as fallback
            memset(img->data, 0, width * height * 4);
            break;
    }
    
    return img;
}

} // extern "C"