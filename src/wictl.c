#include<stdio.h>
#include "wiui.h"
int main(int argc, char *argv[]){

    struct wifi_describe *wifi_table; 
    struct wifi_describe *wifi_table_show;
    wiui *w;
    int n = 0;
    int choose_id;
    char *ssid;
    char *password;
    int wifi_table_length = 3;
    w = get_wifi();
    wifi_table = (*w->scan)(&wifi_table_length);
    wifi_table_show = wifi_table;
    if (argc > 2) {
        printf("There are two ways use wictl\n");
        printf("1, wictl <ssid> <password>\n");
        printf("1, wictl\n");
        return 0;
    }
    if (argc == 0) {
        while (1) {
            printf("Please choose your wifi:\n");
            for (n = 0; n < wifi_table_length; n++) {
                printf("%d, %s\n", n, wifi_table_show->ssid);
                wifi_table_show++;
            }
            scanf("%d", &choose_id);
            if (choose_id <= wifi_table_length && choose_id >= 0) break;
        }
        if (!wifi_table_show->crypto) {
            printf("Please input the wifi password:");
            scanf("%s\n", password);
        }
    } else {
        ssid = argv[1];
        if (argv[2]) password = argv[2];
    }

    if ((*w->connect)(ssid, password)) {
        printf("success\n");
    }
    else {
        printf("failed\n");
    }
    return 0;
}
