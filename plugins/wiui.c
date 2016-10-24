#include "wiui.h"

char* wiui_file_unglob(const char* filename)
{
    glob_t results;
    char* res = NULL;
    results.gl_pathc = 0;
    glob(filename, 0, NULL, &results);
    if (results.gl_pathc == 1)
        res = strdup(results.gl_pathv[0]);
    globfree(&results);
    return res;
}

int  wiui_file_contains(const char* filename, const char* content)
{
    int found = 0; 
    if ((filename == NULL) || (content == NULL)) {
        return 0;
    }    

    char* file = wiui_file_unglob(filename);
    if (file != NULL) {
        size_t len = 1024;
        char* line = calloc(len, sizeof(char));
        if (line == NULL) {
            free(file);
            return 0;
        }    
        FILE* fh = fopen(file, "r");
        if (fh == NULL) {
            free(file);
            free(line);
            return 0;
        }    
        while ((getline(&line, &len, fh) != -1) && (found == 0)) {
            if (strstr(line, content)) {
                found = 1; 
                break;
            }    
        }    
        fclose(fh);
        free(file);
        free(line);
    }
    return found;
}

int wiui_file_exist(const char* filename)
{
    glob_t results;
    results.gl_pathc = 0; 
    glob(filename, 0, NULL, &results);
    int file_found = results.gl_pathc == 1;
    globfree(&results);
    return file_found;
}

int wiui_file_contains_both(const char* filename, const char* content, const char* content2)
{
    int found = 0;
    if ((filename == NULL) || (content == NULL)) {
        return 0;
    }

    char* file = wiui_file_unglob(filename);
    if (file != NULL) {
        size_t len = 1024;
        char* line = calloc(len, sizeof(char));
        if (line == NULL) {
            free(file);
            return 0;
        }
        FILE* fh = fopen(file, "r");
        if (fh == NULL) {
            free(file);
            free(line);
            return 0;
                    }
        while ((getline(&line, &len, fh) != -1) && (found == 0)) {
            if (strstr(line, content) && strstr(line, content2)) {
                found = 1;
                break;
            }
        }
        fclose(fh);
        free(file);
        free(line);
    }
    return found;
}

int wiui_link_targets(const char* filename, const char* targetname)
{
    int size = 100;
    int nchars = 0;
    char* buffer = NULL;
    while (nchars == 0) {
        buffer = (char*) realloc(buffer, size);
        if (buffer == NULL)
            return 0;
        nchars = readlink(filename, buffer, size);
        if (nchars < 0) {
            free(buffer);
            return 0;
        } else {
            buffer[nchars] = '\0';
        }
        if (nchars >= size) {
            size *= 2;
            nchars = 0;
        }
    }
    if (strstr(buffer, targetname)) {
        free(buffer);
        return 1;
    } else {
        free(buffer);        
        return 0;
    }
}


wiui* get_wifi() {
    int platform_type = WIUI_UNKNOWN_PLATFORM;
    wiui *w;
    if (wiui_file_contains("/sys/firmware/devicetree/base/model", "ReSpeaker")) {
        platform_type = WIUI_RESPEAKER;
    }
    else if (wiui_file_contains("/sys/firmware/devicetree/base/model", "BeagleBone")) {
        platform_type = WIUI_BEAGLEBONE;
    }

    switch (platform_type) {
    case WIUI_RESPEAKER:
        w = wiui_respeaker();
        break;
    case WIUI_BEAGLEBONE:
        break;
    }
    
}
