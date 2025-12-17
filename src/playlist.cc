#include "ww.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <algorithm>

extern "C" {

// Load playlist from file
// Supports line-separated file paths and # for comments
int ww_load_playlist(const char *path, char ***files, int *count) {
    if (!path || !files || !count) {
        return -1;
    }
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        return -1;
    }
    
    std::vector<std::string> file_list;
    char line[4096];
    
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        
        // Skip empty lines and comments
        if (len == 0 || line[0] == '#') {
            continue;
        }
        
        // Check if file exists
        struct stat st;
        if (stat(line, &st) == 0 && S_ISREG(st.st_mode)) {
            file_list.push_back(line);
        } else if (stat(line, &st) == 0 && S_ISDIR(st.st_mode)) {
            // If it's a directory, scan for image files
            DIR *dir = opendir(line);
            if (dir) {
                struct dirent *entry;
                while ((entry = readdir(dir)) != nullptr) {
                    if (entry->d_name[0] == '.') {
                        continue; // Skip hidden files
                    }
                    
                    // Build full path
                    std::string full_path = std::string(line) + "/" + entry->d_name;
                    
                    // Check if it's a supported image file
                    const char *ext = strrchr(entry->d_name, '.');
                    if (ext) {
                        ext++; // Skip the dot
                        if (strcasecmp(ext, "png") == 0 ||
                            strcasecmp(ext, "jpg") == 0 ||
                            strcasecmp(ext, "jpeg") == 0 ||
                            strcasecmp(ext, "webp") == 0 ||
                            strcasecmp(ext, "bmp") == 0 ||
                            strcasecmp(ext, "tga") == 0 ||
                            strcasecmp(ext, "tiff") == 0 ||
                            strcasecmp(ext, "tif") == 0 ||
                            strcasecmp(ext, "jxl") == 0 ||
                            strcasecmp(ext, "gif") == 0 ||
                            strcasecmp(ext, "mp4") == 0 ||
                            strcasecmp(ext, "webm") == 0 ||
                            strcasecmp(ext, "ppm") == 0 ||
                            strcasecmp(ext, "pgm") == 0 ||
                            strcasecmp(ext, "pbm") == 0 ||
                            strcasecmp(ext, "pnm") == 0 ||
                            strcasecmp(ext, "ff") == 0) {
                            
                            // Verify it's a regular file
                            if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                                file_list.push_back(full_path);
                            }
                        }
                    }
                }
                closedir(dir);
            }
        }
    }
    
    fclose(fp);
    
    if (file_list.empty()) {
        *files = nullptr;
        *count = 0;
        return 0;
    }
    
    // Sort files alphabetically
    std::sort(file_list.begin(), file_list.end());
    
    // Allocate array of string pointers
    *count = file_list.size();
    *files = (char**)malloc(sizeof(char*) * file_list.size());
    
    if (!*files) {
        *count = 0;
        return -1;
    }
    
    // Copy strings
    for (size_t i = 0; i < file_list.size(); i++) {
        (*files)[i] = strdup(file_list[i].c_str());
        if (!(*files)[i]) {
            // Clean up on failure
            for (size_t j = 0; j < i; j++) {
                free((*files)[j]);
            }
            free(*files);
            *files = nullptr;
            *count = 0;
            return -1;
        }
    }
    
    return 0;
}

// Free playlist
void ww_free_playlist(char **files, int count) {
    if (!files) return;
    
    for (int i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
}

// Get current time period
ww_time_period_t ww_get_current_time_period(void) {
    time_t now = time(nullptr);
    struct tm *local = localtime(&now);
    
    int hour = local->tm_hour;
    
    if (hour >= 6 && hour < 12) {
        return WW_TIME_PERIOD_MORNING;
    } else if (hour >= 12 && hour < 18) {
        return WW_TIME_PERIOD_AFTERNOON;
    } else if (hour >= 18 && hour < 22) {
        return WW_TIME_PERIOD_EVENING;
    } else {
        return WW_TIME_PERIOD_NIGHT;
    }
}

// Get time period name
const char *ww_time_period_name(ww_time_period_t period) {
    switch (period) {
        case WW_TIME_PERIOD_MORNING:
            return "morning";
        case WW_TIME_PERIOD_AFTERNOON:
            return "afternoon";
        case WW_TIME_PERIOD_EVENING:
            return "evening";
        case WW_TIME_PERIOD_NIGHT:
            return "night";
        default:
            return "unknown";
    }
}

} // extern "C"