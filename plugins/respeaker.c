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
    // sleep(5);
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

struct wifi_describe* respeaker_scan(int *count) {
    int i;
    wifi_site_survey("ra0", NULL, 0);
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
static char isStaGetIP(const char *staname) {

    int socket_fd;
    struct sockaddr_in *sin;
    struct ifreq ifr;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        perror("socket error!\n");
        return 0;
    }
    strcpy(ifr.ifr_name, staname);

    if (ioctl(socket_fd, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl error\n");
        return 0;
    } else {
        sin = (struct sockaddr_in *)&(ifr.ifr_addr);
        syslog(LOG_INFO, "current IP = %s\n", inet_ntoa(sin->sin_addr));
        return 1;
    }

}

static struct wifi_describe* wifi_find_ap(const char *name) {
    int i;

    for (i = 0; i < wifi_count; i++) if (!strcmp(name, (char *)st[i].ssid)) return &st[i];

    return NULL;
}


#define lengthof(x) (sizeof(x) / sizeof(x[0]))

/* This function is heavily similar to the wifi_repeater_start in
 * net/wifi_core.c from microd (but changed to call ifdown/ifup instead
 * of fiddling with interface configuration manually. */
static void wifi_repeater_start(const char *ifname, const char *staname, const char *channel, const char *ssid,
                                const char *key, const char *enc, const char *crypto) {
    char buf[100];
    int enctype = 0;

    iwpriv(ifname, "Channel", channel);
    iwpriv(staname, "ApCliEnable", "0");
    if ((strstr(enc, "WPA2PSK") || strstr(enc, "WPAPSKWPA2PSK")) && key) {
        enctype = 1;
        iwpriv(staname, "ApCliAuthMode", "WPA2PSK");
    } else if (strstr(enc, "WPAPSK") && key) {
        enctype = 1;
        iwpriv(staname, "ApCliAuthMode", "WPAPSK");
    } else if (strstr(enc, "WEP") && key) {
        iwpriv(staname, "ApCliAuthMode", "AUTOWEP");
        iwpriv(staname, "ApCliEncrypType", "WEP");
        iwpriv(staname, "ApCliDefaultKeyID", "1");
        iwpriv(staname, "ApCliKey1", key);
        iwpriv(staname, "ApCliSsid", ssid);
    } else if (!key || key[0] == '\0') {
        iwpriv(staname, "ApCliAuthMode", "NONE");
        iwpriv(staname, "ApCliSsid", ssid);
    } else {
        return;
    }

    if (enctype) {
        if (strstr(crypto, "AES") || strstr(crypto, "TKIPAES")) iwpriv(staname, "ApCliEncrypType", "AES");
        else iwpriv(staname, "ApCliEncrypType", "TKIP");
        iwpriv(staname, "ApCliSsid", ssid);
        iwpriv(staname, "ApCliWPAPSK", key);
    }
    iwpriv(staname, "ApCliEnable", "1");
    snprintf(buf, lengthof(buf) - 1, "ifconfig '%s' up", staname);
    system(buf);
}
static int setDefaultSta(const char *ifname, const char *staname, char *essid, char *passwd) {
    int try_count = 0;
    while (1) {
        struct wifi_describe *c;
        wifi_site_survey(ifname, essid, 0);
        c = wifi_find_ap(essid);
        try_count++;
        if (c) {
            syslog(LOG_INFO, "Found network, trying to associate (essid: %s, bssid: %s, channel: %s, enc: %s, crypto: %s)\n",
                   essid, c->ssid, c->channel, c->security, c->crypto);

            wifi_repeater_start(ifname, staname, c->channel, essid, passwd, c->security, c->crypto);
            if (isStaGetIP(staname)) return 1;
        } else {
            syslog(LOG_INFO, "No signal found to connect to\n");
        }
        if (try_count == 3) return 0;
        sleep(1);
    }
}

int respeaker_connect(const char *ssid, const char *passwd) {
    return setDefaultSta("ra0", "apcli0", ssid, passwd);
}
wiui* wiui_respeaker() {
    wiui *w = (wiui *)calloc(1, sizeof(wiui));
    openlog("wiui", 0, 0);
    if (w == NULL) return NULL;
    w->scan = &respeaker_scan;
    w->connect = &respeaker_connect;

    return w;
}
