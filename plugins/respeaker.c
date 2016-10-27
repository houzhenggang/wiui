#include "wiui.h"


#include <signal.h>

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <glob.h>
#include <errno.h>
#include <sys/sysmacros.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <linux/un.h>
#include <poll.h>
#include <assert.h>
#include <linux/if.h>
#include <linux/types.h>
#include <linux/wireless.h>
#include <syslog.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static struct wifi_describe st[64];
static int wifi_count = 0;

#define RTPRIV_IOCTL_SET (SIOCIWFIRSTPRIV + 0x02)
static void iwpriv(const char *name, const char *key, const char *val) {
    int socket_id;
    struct iwreq wrq;
    char data[64];

    snprintf(data, 64, "%s=%s", key, val);
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(wrq.ifr_ifrn.ifrn_name, name);
    wrq.u.data.length = strlen(data);
    wrq.u.data.pointer = data;
    wrq.u.data.flags = 0;
    ioctl(socket_id, RTPRIV_IOCTL_SET, &wrq);
    close(socket_id);
}
static void next_field(char **line, char *output, int n) {
    char *l = *line;
    int i;

    memcpy(output, *line, n);
    *line = &l[n];

    for (i = n - 1; i > 0; i--) {
        if (output[i] != ' ') break;
        output[i] = '\0';
    }
}

#define RTPRIV_IOCTL_GSITESURVEY (SIOCIWFIRSTPRIV + 0x0D)
static void wifi_site_survey(const char *ifname, const char *essid, int print) {
    char *s = malloc(IW_SCAN_MAX_DATA);
    int ret;
    int socket_id;
    struct iwreq wrq;
    char *line, *start;

    iwpriv(ifname, "SiteSurvey", (essid ? essid : ""));
    sleep(5);
    memset(s, 0x00, IW_SCAN_MAX_DATA);
    strcpy(wrq.ifr_name, ifname);
    wrq.u.data.length = IW_SCAN_MAX_DATA;
    wrq.u.data.pointer = s;
    wrq.u.data.flags = 0;
    socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    ret = ioctl(socket_id, RTPRIV_IOCTL_GSITESURVEY, &wrq);
    close(socket_id);
    if (ret != 0) goto out;

    if (wrq.u.data.length < 1) goto out;

    /* ioctl result starts with a newline, for some reason */
    start = s;
    while (*start == '\n') start++;

    line = strtok((char *)start, "\n");
    line = strtok(NULL, "\n");
    
    wifi_count = 0;
    while (line && (wifi_count < 64)) {
        next_field(&line, st[wifi_count].channel, sizeof(st->channel));
        next_field(&line, st[wifi_count].ssid, sizeof(st->ssid));
        next_field(&line, st[wifi_count].bssid, sizeof(st->bssid));
        next_field(&line, st[wifi_count].security, sizeof(st->security));
        next_field(&line, st[wifi_count].siganl, sizeof(st->siganl));
        line = strtok(NULL, "\n");
        st[wifi_count].crypto = strstr(st[wifi_count].security, "/");
        if (st[wifi_count].crypto) {
            *st[wifi_count].crypto = '\0';
            st[wifi_count].crypto++;
            //syslog(LOG_INFO, "Found network - %s %s %s %s %s\n",
            //       st[wifi_count].channel, st[wifi_count].ssid, st[wifi_count].bssid, st[wifi_count].security, st[wifi_count].siganl);
        } else {
            st[wifi_count].crypto = "";
        }
        wifi_count++;
    }

    if (wifi_count == 0 && !print) syslog(LOG_INFO, "No results");
out:
    free(s);
}

struct wifi_describe * respeaker_scan(int *count) {
    int i;
    wifi_site_survey("ra0",NULL,0);
    struct wifi_describe *wifi_log;
    struct wifi_describe *wifi = (struct wifi_describe *)malloc(wifi_count * (sizeof(struct wifi_describe)));
    memcpy(wifi, st, wifi_count * (sizeof(struct wifi_describe))); 
    wifi_log = wifi;
#if 1
    for (i = 0; i < wifi_count; i++) {
        syslog(LOG_INFO, "Found network - %s %s %s %s %s\n",
       wifi_log->channel, wifi_log->ssid, wifi_log->bssid, wifi_log->security, wifi_log->siganl);
        wifi_log++;
    }
#endif
    *count = wifi_count;
    return  wifi;
}

int respeaker_connect(const char *ssid, const char *passwd) {

}
wiui* wiui_respeaker() {
    wiui* w = (wiui*) calloc(1, sizeof(wiui));
    openlog("wiui", 0, 0);
    if (w == NULL)
        return NULL;
    w->scan = &respeaker_scan;
    w->connect = &respeaker_connect;

    return w;
}
