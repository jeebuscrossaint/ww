#include "ww.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>
#include <vector>
#include <string>

extern void set_error(const char *msg);

// slideshow state structure
struct slideshow_state_t {
    bool enabled;
    int interval;
    bool random;
    bool recursive;
    int transition;
    float duration;
    int fps;
    std::vector<std::string> files;
    size_t current_index;
};

static const char* get_cache_dir() 
{
    static char cache_dir[512] = {0};
    if (cache_dir[0] != '\0')
        return cache_dir;
    
    const char* xdg_cache = getenv("XDG_CACHE_HOME");
    if (xdg_cache && xdg_cache[0] != '\0')
        snprintf(cache_dir, sizeof(cache_dir), "%s/ww", xdg_cache);
    else {
        const char* home = getenv("HOME");
        if (!home) {
            struct passwd* pw = getpwuid(getuid());
            home = pw ? pw->pw_dir : "/tmp";
        }
        snprintf(cache_dir, sizeof(cache_dir), "%s/.cache/ww", home);
    }
    
    mkdir(cache_dir, 0755);
    return cache_dir;
}

int ww_cache_save(const char* output_name, const ww_config_t* config) 
{
    if (!output_name || !config) {
        set_error("NULL parameter");
        return -1;
    }
    
    const char* cache_dir = get_cache_dir();
    char cache_path[768];
    snprintf(cache_path, sizeof(cache_path), "%s/%s", cache_dir, output_name);
    
    FILE* f = fopen(cache_path, "w");
    if (!f) {
        set_error("Failed to open cache file for writing");
        return -1;
    }
    
    // convert relative paths to absolute
    char abs_path[PATH_MAX];
    const char* path_to_save = config->file_path;
    if (config->file_path && config->file_path[0] != '\0' && config->file_path[0] != '/') {
        if (realpath(config->file_path, abs_path))
            path_to_save = abs_path;
    }
    
    fprintf(f, "path=%s\n", path_to_save ? path_to_save : "");
    fprintf(f, "type=%d\n", config->type);
    fprintf(f, "mode=%d\n", config->mode);
    fprintf(f, "bg_color=0x%08X\n", config->bg_color);
    fprintf(f, "loop=%d\n", config->loop ? 1 : 0);
    fprintf(f, "transition=%d\n", config->transition);
    fprintf(f, "transition_duration=%.2f\n", config->transition_duration);
    fprintf(f, "transition_fps=%d\n", config->transition_fps);
    
    fclose(f);
    return 0;
}

int ww_cache_save_slideshow(const char* output_name, bool enabled, int interval, 
                            bool random, bool recursive, int mode, int transition, 
                            float duration, int fps, const char** files, 
                            int file_count, int current_index)
{
    if (!output_name) {
        set_error("NULL parameter");
        return -1;
    }
    
    const char* cache_dir = get_cache_dir();
    char cache_path[768];
    snprintf(cache_path, sizeof(cache_path), "%s/%s.slideshow", cache_dir, output_name);
    
    FILE* f = fopen(cache_path, "w");
    if (!f) {
        set_error("Failed to open slideshow cache file");
        return -1;
    }
    
    fprintf(f, "enabled=%d\n", enabled ? 1 : 0);
    fprintf(f, "interval=%d\n", interval);
    fprintf(f, "random=%d\n", random ? 1 : 0);
    fprintf(f, "recursive=%d\n", recursive ? 1 : 0);
    fprintf(f, "mode=%d\n", mode);
    fprintf(f, "transition=%d\n", transition);
    fprintf(f, "duration=%.2f\n", duration);
    fprintf(f, "fps=%d\n", fps);
    fprintf(f, "current_index=%d\n", current_index);
    fprintf(f, "file_count=%d\n", file_count);
    
    for (int i = 0; i < file_count; i++) {
        fprintf(f, "file=%s\n", files[i]);
    }
    
    fclose(f);
    return 0;
}

int ww_cache_load_slideshow(const char* output_name, bool* enabled, int* interval,
                            bool* random, bool* recursive, int* mode, int* transition,
                            float* duration, int* fps, char*** files,
                            int* file_count, int* current_index)
{
    if (!output_name) {
        set_error("NULL parameter");
        return -1;
    }
    
    const char* cache_dir = get_cache_dir();
    char cache_path[768];
    snprintf(cache_path, sizeof(cache_path), "%s/%s.slideshow", cache_dir, output_name);
    
    FILE* f = fopen(cache_path, "r");
    if (!f)
        return -1;
    
    std::vector<std::string> file_list;
    char line[2048];
    int en = 0, rnd = 0, rec = 0, md = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "enabled=%d", &en) == 1)
            *enabled = en != 0;
        else if (sscanf(line, "interval=%d", interval) == 1) {}
        else if (sscanf(line, "random=%d", &rnd) == 1)
            *random = rnd != 0;
        else if (sscanf(line, "recursive=%d", &rec) == 1)
            *recursive = rec != 0;
        else if (sscanf(line, "mode=%d", &md) == 1)
            *mode = md;
        else if (sscanf(line, "transition=%d", transition) == 1) {}
        else if (sscanf(line, "duration=%f", duration) == 1) {}
        else if (sscanf(line, "fps=%d", fps) == 1) {}
        else if (sscanf(line, "current_index=%d", current_index) == 1) {}
        else if (sscanf(line, "file_count=%d", file_count) == 1) {}
        else if (strncmp(line, "file=", 5) == 0) {
            char* path = line + 5;
            char* newline = strchr(path, '\n');
            if (newline) *newline = '\0';
            file_list.push_back(path);
        }
    }
    
    fclose(f);
    
    if (!file_list.empty()) {
        *files = (char**)malloc(sizeof(char*) * file_list.size());
        if (!*files) {
            set_error("Memory allocation failed");
            return -1;
        }
        
        for (size_t i = 0; i < file_list.size(); i++) {
            (*files)[i] = strdup(file_list[i].c_str());
        }
        *file_count = file_list.size();
    }
    
    return 0;
}

int ww_cache_load(const char* output_name, ww_config_t* config) 
{
    if (!output_name || !config) {
        set_error("NULL parameter");
        return -1;
    }
    
    const char* cache_dir = get_cache_dir();
    char cache_path[768];
    snprintf(cache_path, sizeof(cache_path), "%s/%s", cache_dir, output_name);
    
    FILE* f = fopen(cache_path, "r");
    if (!f)
        return -1;
    
    static char path_buffer[1024];
    int type, mode, loop;
    unsigned int bg_color;
    
    char line[1280];
    int trans = 0;
    float trans_dur = 0;
    int trans_fps = 0;
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "path=", 5) == 0) {
            strncpy(path_buffer, line + 5, sizeof(path_buffer) - 1);
            path_buffer[sizeof(path_buffer) - 1] = '\0';
            char* newline = strchr(path_buffer, '\n');
            if (newline) *newline = '\0';
            config->file_path = path_buffer;
        } 
        else if (sscanf(line, "type=%d", &type) == 1)
            config->type = (ww_filetype_t)type;
        else if (sscanf(line, "mode=%d", &mode) == 1)
            config->mode = (ww_scale_mode_t)mode;
        else if (sscanf(line, "bg_color=0x%X", &bg_color) == 1)
            config->bg_color = bg_color;
        else if (sscanf(line, "loop=%d", &loop) == 1)
            config->loop = loop != 0;
        else if (sscanf(line, "transition=%d", &trans) == 1)
            config->transition = (ww_transition_type_t)trans;
        else if (sscanf(line, "transition_duration=%f", &trans_dur) == 1)
            config->transition_duration = trans_dur;
        else if (sscanf(line, "transition_fps=%d", &trans_fps) == 1)
            config->transition_fps = trans_fps;
    }
    
    fclose(f);
    return 0;
}

void ww_cache_clear(const char* output_name) 
{
    if (!output_name) 
        return;
    
    const char* cache_dir = get_cache_dir();
    char cache_path[768];
    snprintf(cache_path, sizeof(cache_path), "%s/%s", cache_dir, output_name);
    unlink(cache_path);
}