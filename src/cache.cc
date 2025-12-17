#include "ww.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <limits.h>

extern void set_error(const char *msg);

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
    
    fclose(f);
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