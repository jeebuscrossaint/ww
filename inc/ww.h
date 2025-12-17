#ifndef WW_H
#define WW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* Version info */
#define WW_VERSION_MAJOR 0
#define WW_VERSION_MINOR 1
#define WW_VERSION_PATCH 0

/* Supported file types */
typedef enum {
    WW_TYPE_UNKNOWN = 0,
    WW_TYPE_PNG,
    WW_TYPE_JPEG,
    WW_TYPE_WEBP,
    WW_TYPE_BMP,
    WW_TYPE_TGA,
    WW_TYPE_PNM,
    WW_TYPE_TIFF,
    WW_TYPE_JXL,
    WW_TYPE_FARBFELD,
    WW_TYPE_GIF,
    WW_TYPE_MP4,
    WW_TYPE_WEBM,
    WW_TYPE_SOLID_COLOR,
} ww_filetype_t;

/* Scaling modes */
typedef enum {
    WW_MODE_FIT = 0,      // Scale to fit with letterboxing (default)
    WW_MODE_FILL,         // Scale to fill, crop if needed
    WW_MODE_STRETCH,      // Stretch to fill, ignore aspect ratio
    WW_MODE_CENTER,       // No scaling, center image
    WW_MODE_TILE,         // Repeat image to fill
} ww_scale_mode_t;

/* Image filter types */
typedef struct {
    float blur;           // Blur radius (0 = no blur)
    float brightness;     // Brightness multiplier (1.0 = normal, 0.0 = black, 2.0 = double)
    float contrast;       // Contrast multiplier (1.0 = normal, 0.0 = gray, 2.0 = double)
    float saturation;     // Saturation multiplier (1.0 = normal, 0.0 = grayscale, 2.0 = double)
} ww_filter_t;

/* Gradient types */
typedef enum {
    WW_GRADIENT_LINEAR = 0,
    WW_GRADIENT_RADIAL,
    WW_GRADIENT_ANGULAR,
} ww_gradient_type_t;

/* Gradient stop */
typedef struct {
    float position;       // 0.0 to 1.0
    uint32_t color;       // RGBA color
} ww_gradient_stop_t;

/* Gradient configuration */
typedef struct {
    ww_gradient_type_t type;
    float angle;          // For linear gradients (degrees)
    float center_x;       // For radial/angular gradients (0.0 to 1.0)
    float center_y;       // For radial/angular gradients (0.0 to 1.0)
    ww_gradient_stop_t *stops;
    int stop_count;
} ww_gradient_config_t;

/* Transition types */
typedef enum {
    WW_TRANSITION_NONE = 0,
    WW_TRANSITION_FADE,
    WW_TRANSITION_SLIDE_LEFT,
    WW_TRANSITION_SLIDE_RIGHT,
    WW_TRANSITION_SLIDE_UP,
    WW_TRANSITION_SLIDE_DOWN,
    WW_TRANSITION_ZOOM_IN,
    WW_TRANSITION_ZOOM_OUT,
    WW_TRANSITION_CIRCLE_OPEN,
    WW_TRANSITION_CIRCLE_CLOSE,
    WW_TRANSITION_WIPE_LEFT,
    WW_TRANSITION_WIPE_RIGHT,
    WW_TRANSITION_WIPE_UP,
    WW_TRANSITION_WIPE_DOWN,
    WW_TRANSITION_DISSOLVE,
    WW_TRANSITION_PIXELATE,
} ww_transition_type_t;

/* Time period for time-based switching */
typedef enum {
    WW_TIME_PERIOD_MORNING = 0,   // 6:00 - 12:00
    WW_TIME_PERIOD_AFTERNOON,     // 12:00 - 18:00
    WW_TIME_PERIOD_EVENING,       // 18:00 - 22:00
    WW_TIME_PERIOD_NIGHT,         // 22:00 - 6:00
} ww_time_period_t;

/* Output information */
typedef struct {
    char *name;
    int32_t width;
    int32_t height;
    int32_t refresh_rate;
    int32_t scale;
} ww_output_t;

/* Wallpaper configuration */
typedef struct {
    const char *file_path;
    ww_filetype_t type;
    const char *output_name;  /* NULL for all outputs */
    bool loop;                 /* For videos/gifs */
    ww_scale_mode_t mode;      /* Scaling mode */
    uint32_t bg_color;         /* Background color (RGBA) for solid_color or letterboxing */
    ww_filter_t *filter;       /* Optional image filter */
    ww_transition_type_t transition;  /* Transition effect */
    float transition_duration; /* Transition duration in seconds */
    int transition_fps;        /* Target FPS for transitions (default: 30) */
} ww_config_t;

/* Image data structure (opaque) */
typedef struct image_data_t image_data_t;

/* Video decoder structure (opaque) */
typedef struct video_decoder_t video_decoder_t;

/* Main API functions */
int ww_init(void);
void ww_cleanup(void);

ww_filetype_t ww_detect_filetype(const char *path);
int ww_set_wallpaper(const ww_config_t *config);
int ww_set_wallpaper_no_loop(const ww_config_t *config);
int ww_list_outputs(ww_output_t **outputs, int *count);
void ww_dispatch_events(void);

/* Image loading functions */
image_data_t *ww_load_image(const char *path, int output_width, int output_height, bool preserve_aspect);
image_data_t *ww_load_image_mode(const char *path, int output_width, int output_height, int mode, uint32_t bg_color);
void ww_free_image(image_data_t *img);

/* Video decoder functions */
video_decoder_t *ww_video_create(const char *path, int target_width, int target_height, bool loop);
image_data_t *ww_video_next_frame(video_decoder_t *decoder);
double ww_video_get_frame_duration(video_decoder_t *decoder);
bool ww_video_is_eof(video_decoder_t *decoder);
void ww_video_seek_start(video_decoder_t *decoder);
void ww_video_destroy(video_decoder_t *decoder);

/* Transition state structure (opaque) */
typedef struct ww_transition_state ww_transition_state;

/* Transition functions */
ww_transition_state *ww_transition_create(ww_transition_type_t type, float duration,
                                          int width, int height);
void ww_transition_destroy(ww_transition_state *state);
void ww_transition_start(ww_transition_state *state, const uint8_t *old_data,
                         const uint8_t *new_data);
bool ww_transition_update(ww_transition_state *state, float delta_time, uint8_t **output_data);
bool ww_transition_is_active(const ww_transition_state *state);
float ww_transition_get_progress(const ww_transition_state *state);

/* Directory scanning */
typedef struct {
    char **paths;
    int count;
} ww_file_list_t;

/* Scan directory for image files */
int ww_scan_directory(const char *dir_path, ww_file_list_t *file_list, bool recursive);

/* Free file list */
void ww_free_file_list(ww_file_list_t *file_list);

/* Error handling */
const char *ww_get_error(void);

#ifdef __cplusplus
}
#endif

#endif /* WW_H */