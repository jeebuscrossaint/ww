#include <iostream>
#include <cstring>
#include <cstdlib>
#include <getopt.h>
#include <libgen.h>
#include <vector>
#include <algorithm>
#include <random>
#include <signal.h>
#include <unistd.h>

#include "ww.h"

// Global state for slideshow
static volatile bool running = true;
static volatile bool needs_update = false;

static void alarm_handler(int signum) {
    (void)signum;
    needs_update = true;
}

static void signal_handler(int signum) {
    (void)signum;
    running = false;
}

static void print_version() {
    std::cout << "ww v" << WW_VERSION_MAJOR << "." 
              << WW_VERSION_MINOR << "." 
              << WW_VERSION_PATCH << std::endl;
    std::cout << "Universal Wayland wallpaper setter" << std::endl;
}

static uint32_t parse_color(const char *color_str) {
    if (!color_str) return 0xFF000000;
    
    // Skip '#' if present
    if (color_str[0] == '#') {
        color_str++;
    }
    
    unsigned int r, g, b, a = 255;
    int len = strlen(color_str);
    
    if (len == 6) {
        // RGB format
        sscanf(color_str, "%02x%02x%02x", &r, &g, &b);
    } else if (len == 8) {
        // RGBA format
        sscanf(color_str, "%02x%02x%02x%02x", &r, &g, &b, &a);
    } else {
        return 0xFF000000; // Default black
    }
    
    // Return as RGBA
    return (r << 24) | (g << 16) | (b << 8) | a;
}

static void print_usage(const char *prog_name) {
    // Use basename to show just "ww" instead of full path
    char *prog_copy = strdup(prog_name);
    char *base = basename(prog_copy);
    
    std::cout << "Usage: " << base << " [OPTIONS] <file|color>" << std::endl;
    std::cout << "\nOptions:" << std::endl;
    std::cout << "  -o, --output <name>    Set wallpaper for specific output" << std::endl;
    std::cout << "  -m, --mode <mode>      Scaling mode: fit, fill, stretch, center, tile (default: fit)" << std::endl;
    std::cout << "  -c, --color <#RRGGBB>  Solid color background or letterbox color" << std::endl;
    std::cout << "  -l, --loop             Loop animated wallpapers (GIF/video)" << std::endl;
    std::cout << "  -S, --slideshow        Slideshow mode (multiple files)" << std::endl;
    std::cout << "  -i, --interval <sec>   Slideshow interval in seconds (default: 300)" << std::endl;
    std::cout << "  -r, --random           Random slideshow order" << std::endl;
    std::cout << "  -L, --list-outputs     List available outputs" << std::endl;
    std::cout << "  -v, --version          Show version information" << std::endl;
    std::cout << "  -h, --help             Show this help message" << std::endl;
    std::cout << "\nScaling modes:" << std::endl;
    std::cout << "  fit      - Scale to fit with letterboxing (default)" << std::endl;
    std::cout << "  fill     - Scale to fill, crop if needed" << std::endl;
    std::cout << "  stretch  - Stretch to fill, ignore aspect ratio" << std::endl;
    std::cout << "  center   - No scaling, center image" << std::endl;
    std::cout << "  tile     - Repeat image to fill screen" << std::endl;
    std::cout << "\nSupported formats:" << std::endl;
    std::cout << "  Static Images: PNG, JPEG, BMP, TGA, PNM/PBM/PGM/PPM" << std::endl;
    std::cout << "                 WebP, TIFF/TIF, JXL (JPEG XL), Farbfeld" << std::endl;
    std::cout << "  Animated: GIF, MP4, WebM, Animated WebP" << std::endl;
    std::cout << "\nExamples:" << std::endl;
    std::cout << "  " << base << " wallpaper.png" << std::endl;
    std::cout << "  " << base << " --loop video.mp4" << std::endl;
    std::cout << "  " << base << " --mode fill image.jpg" << std::endl;
    std::cout << "  " << base << " --mode center --color '#282828' logo.png" << std::endl;
    std::cout << "  " << base << " --color '#FF5733'" << std::endl;
    std::cout << "  " << base << " -S -i 300 img1.jpg img2.png img3.webp" << std::endl;
    std::cout << "  " << base << " -S -r -i 60 ~/wallpapers/*.jpg" << std::endl;
    
    free(prog_copy);
}

static void list_outputs() {
    ww_output_t *outputs = nullptr;
    int count = 0;

    if (ww_list_outputs(&outputs, &count) != 0) {
        std::cerr << "Error: Failed to list outputs: " << ww_get_error() << std::endl;
        return;
    }

    std::cout << "Available outputs:" << std::endl;
    for (int i = 0; i < count; i++) {
        std::cout << "  " << outputs[i].name 
                  << " (" << outputs[i].width << "x" << outputs[i].height 
                  << "@" << outputs[i].refresh_rate << "Hz"
                  << ", scale=" << outputs[i].scale << ")" << std::endl;
    }

    free(outputs);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Configuration
    ww_config_t config = {
        .file_path = nullptr,
        .type = WW_TYPE_UNKNOWN,
        .output_name = nullptr,
        .loop = false,
        .mode = WW_MODE_FIT,
        .bg_color = 0x000000FF, // Black with full alpha
    };

    // Slideshow options
    bool slideshow_mode = false;
    int slideshow_interval = 300;  // 5 minutes default
    bool random_mode = false;
    std::vector<std::string> files;

    // Parse command line arguments
    static struct option long_options[] = {
        {"output",        required_argument, 0, 'o'},
        {"mode",          required_argument, 0, 'm'},
        {"color",         required_argument, 0, 'c'},
        {"loop",          no_argument,       0, 'l'},
        {"slideshow",     no_argument,       0, 'S'},
        {"interval",      required_argument, 0, 'i'},
        {"random",        no_argument,       0, 'r'},
        {"list-outputs",  no_argument,       0, 'L'},
        {"version",       no_argument,       0, 'v'},
        {"help",          no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    bool list_mode = false;
    bool color_only = false;

    while ((opt = getopt_long(argc, argv, "o:m:c:lSi:rLvh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                config.output_name = optarg;
                break;
            case 'm':
                if (strcmp(optarg, "fit") == 0) {
                    config.mode = WW_MODE_FIT;
                } else if (strcmp(optarg, "fill") == 0) {
                    config.mode = WW_MODE_FILL;
                } else if (strcmp(optarg, "stretch") == 0) {
                    config.mode = WW_MODE_STRETCH;
                } else if (strcmp(optarg, "center") == 0) {
                    config.mode = WW_MODE_CENTER;
                } else if (strcmp(optarg, "tile") == 0) {
                    config.mode = WW_MODE_TILE;
                } else {
                    std::cerr << "Error: Invalid mode '" << optarg << "'" << std::endl;
                    std::cerr << "Valid modes: fit, fill, stretch, center, tile" << std::endl;
                    return 1;
                }
                break;
            case 'c':
                config.bg_color = parse_color(optarg);
                color_only = true;
                break;
            case 'l':
                config.loop = true;
                break;
            case 'S':
                slideshow_mode = true;
                break;
            case 'i':
                slideshow_interval = atoi(optarg);
                if (slideshow_interval <= 0) {
                    std::cerr << "Error: Invalid interval" << std::endl;
                    return 1;
                }
                break;
            case 'r':
                random_mode = true;
                break;
            case 'L':
                list_mode = true;
                break;
            case 'v':
                print_version();
                return 0;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    // Initialize Wayland connection
    if (ww_init() != 0) {
        std::cerr << "Error: Failed to initialize: " << ww_get_error() << std::endl;
        return 1;
    }

    // Handle list-outputs mode
    if (list_mode) {
        list_outputs();
        ww_cleanup();
        return 0;
    }

    // Get file path(s) from remaining arguments (or use solid color)
    if (optind >= argc) {
        if (!color_only) {
            std::cerr << "Error: No file or color specified" << std::endl;
            print_usage(argv[0]);
            ww_cleanup();
            return 1;
        }
        // Solid color mode
        config.type = WW_TYPE_SOLID_COLOR;
        config.file_path = nullptr;
    } else {
        // Collect all file arguments
        for (int i = optind; i < argc; i++) {
            files.push_back(argv[i]);
        }

        // Check slideshow requirements
        if (slideshow_mode && files.size() < 2) {
            std::cerr << "Error: Slideshow mode requires at least 2 files" << std::endl;
            ww_cleanup();
            return 1;
        }

        // Shuffle if random mode
        if (random_mode && files.size() > 1) {
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(files.begin(), files.end(), g);
        }

        // Use first file initially
        config.file_path = files[0].c_str();

        // Detect file type
        config.type = ww_detect_filetype(config.file_path);
        if (config.type == WW_TYPE_UNKNOWN) {
            std::cerr << "Error: Unsupported file type: " << config.file_path << std::endl;
            ww_cleanup();
            return 1;
        }
    }

    // Set the wallpaper (non-blocking for slideshow, blocking otherwise)
    if (slideshow_mode) {
        if (ww_set_wallpaper_no_loop(&config) != 0) {
            std::cerr << "Error: Failed to set wallpaper: " << ww_get_error() << std::endl;
            ww_cleanup();
            return 1;
        }
    } else {
        if (ww_set_wallpaper(&config) != 0) {
            std::cerr << "Error: Failed to set wallpaper: " << ww_get_error() << std::endl;
            ww_cleanup();
            return 1;
        }
        std::cout << "Wallpaper set successfully!" << std::endl;
        ww_cleanup();
        return 0;
    }

    // Slideshow mode - keep running and switch wallpapers
    std::cout << "Slideshow started with " << files.size() << " files (interval: " 
              << slideshow_interval << "s, random: " << (random_mode ? "yes" : "no") << ")" << std::endl;

    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGALRM, alarm_handler);

    size_t current_index = 0;
    alarm(slideshow_interval);

    while (running) {
        if (needs_update) {
            needs_update = false;
            
            // Move to next image
            if (random_mode) {
                // Pick random image (different from current)
                size_t new_index = current_index;
                if (files.size() > 1) {
                    std::random_device rd;
                    std::mt19937 g(rd());
                    std::uniform_int_distribution<size_t> dist(0, files.size() - 1);
                    do {
                        new_index = dist(g);
                    } while (new_index == current_index);
                }
                current_index = new_index;
            } else {
                // Sequential
                current_index = (current_index + 1) % files.size();
            }

            // Update wallpaper
            config.file_path = files[current_index].c_str();
            config.type = ww_detect_filetype(config.file_path);
            
            if (config.type != WW_TYPE_UNKNOWN) {
                std::cout << "Switching to: " << files[current_index] << std::endl;
                
                // Set new wallpaper without blocking
                if (ww_set_wallpaper_no_loop(&config) != 0) {
                    std::cerr << "Warning: Failed to set wallpaper: " << ww_get_error() << std::endl;
                }
            }

            // Set next alarm
            alarm(slideshow_interval);
        }

        // Dispatch wayland events (non-blocking)
        ww_dispatch_events();
        
        // Sleep briefly to avoid busy loop
        usleep(100000); // 100ms
    }

    std::cout << "\nSlideshow stopped" << std::endl;
    ww_cleanup();
    return 0;
}