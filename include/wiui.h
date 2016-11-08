#ifndef  _WIFI_H_
#define  _WIFI_H_

#include <stddef.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <pwd.h>
#include <ftw.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <glob.h>



enum {
    WIUI_UNKNOWN_PLATFORM,
    WIUI_RESPEAKER,
    WIUI_BEAGLEBONE,
    WIUI_RESPBERRY_PI,
};

struct wifi_describe{
    char channel[4];
    char ssid[32];
    char bssid[20];
    char security[23];
    char *crypto;
    char siganl[9];
};

typedef  struct {
    struct wifi_describe* (*scan)(int *count);
    int (*connect)(struct wifi_describe *wifi, const char* password);
}wiui;
wiui* get_wifi();
int  wiui_file_contains(const char *filename, const char *content);
char* wiui_file_unglob(const char *filename);
int wiui_file_exist(const char *filename);

wiui* wiui_respeaker();
#endif
