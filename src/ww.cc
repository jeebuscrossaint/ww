#include "ww.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>

// ============================================================================
// Error Handling
// ============================================================================

static thread_local char error_buffer[256] = {0};

void set_error(const char *msg) {
    snprintf(error_buffer, sizeof(error_buffer), "%s", msg);
}

const char *ww_get_error(void) {
    return error_buffer;
}

// ============================================================================
// File Type Detection
// ============================================================================

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

// ============================================================================
// Directory Scanning
// ============================================================================

// Helper function to check if a file is a supported image format
static bool is_supported_image(const char *filename) {
    // Use existing filetype detection
    ww_filetype_t type = ww_detect_filetype(filename);
    return type != WW_TYPE_UNKNOWN;
}

// Recursive directory scanning helper
static void scan_directory_recursive(const char *dir_path, std::vector<std::string> &files, bool recursive) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        return;
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Build full path
        std::string full_path = std::string(dir_path) + "/" + entry->d_name;
        
        // Check if it's a directory or file
        struct stat st;
        if (stat(full_path.c_str(), &st) != 0) {
            continue;
        }
        
        if (S_ISDIR(st.st_mode) && recursive) {
            // Recursively scan subdirectories
            scan_directory_recursive(full_path.c_str(), files, recursive);
        } else if (S_ISREG(st.st_mode) && is_supported_image(entry->d_name)) {
            // Add supported image files
            files.push_back(full_path);
        }
    }
    
    closedir(dir);
}

// Public API for directory scanning
int ww_scan_directory(const char *dir_path, ww_file_list_t *file_list, bool recursive) {
    if (!dir_path || !file_list) {
        set_error("NULL pointer provided");
        return -1;
    }
    
    // Check if directory exists
    struct stat st;
    if (stat(dir_path, &st) != 0) {
        set_error("Directory does not exist");
        return -1;
    }
    
    if (!S_ISDIR(st.st_mode)) {
        set_error("Path is not a directory");
        return -1;
    }
    
    // Scan directory
    std::vector<std::string> files;
    scan_directory_recursive(dir_path, files, recursive);
    
    if (files.empty()) {
        set_error("No supported image files found in directory");
        return -1;
    }
    
    // Sort files alphabetically for consistent ordering
    std::sort(files.begin(), files.end());
    
    // Allocate and copy file paths
    file_list->count = files.size();
    file_list->paths = (char **)malloc(sizeof(char *) * files.size());
    
    if (!file_list->paths) {
        set_error("Memory allocation failed");
        return -1;
    }
    
    for (size_t i = 0; i < files.size(); i++) {
        file_list->paths[i] = strdup(files[i].c_str());
        if (!file_list->paths[i]) {
            // Clean up on failure
            for (size_t j = 0; j < i; j++) {
                free(file_list->paths[j]);
            }
            free(file_list->paths);
            set_error("Memory allocation failed");
            return -1;
        }
    }
    
    return 0;
}

// Free file list
void ww_free_file_list(ww_file_list_t *file_list) {
    if (!file_list) {
        return;
    }
    
    if (file_list->paths) {
        for (int i = 0; i < file_list->count; i++) {
            free(file_list->paths[i]);
        }
        free(file_list->paths);
    }
    
    file_list->paths = nullptr;
    file_list->count = 0;
}

