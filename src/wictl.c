#include<stdio.h>
#include "wiui.h"

int main(int argc, char *argv[]) {

    struct wifi_describe *wifi_table; 
    struct wifi_describe *wifi_table_show;
    wiui *w;
    int n = 0;
    int choose_id;
    char ssid[255];
    char password[255];
    int wifi_table_length = 3;
    w = get_wifi();
    if (argc > 4) {
        printf("There are two ways use wictl\n");
        printf("1, wictl <ssid> <password>\n");
        printf("2, wictl\n");
        return 0;
    }
    if (argc == 1) {
        while (1) {
            wifi_table = (*w->scan)(&wifi_table_length);
            wifi_table_show = wifi_table;
            for (n = 0; n < wifi_table_length; n++) {
                printf("%d, %s\n", n, wifi_table_show->ssid);
                wifi_table_show++;
            }
            printf("Please choose your wifi: ");
            scanf("%d", &choose_id);
            if (choose_id < wifi_table_length && choose_id >= 0) break;
        }

        wifi_table = wifi_table + choose_id;
        if (*(wifi_table->crypto) != 0) {
            printf("Please input the wifi password: ");
            scanf("%s", password);
        }
        strcpy(ssid, wifi_table->ssid);
    } else {
        strcpy(ssid, argv[1]);
        if (argv[2]) strcpy(password , argv[2]);

        wifi_table = (*w->scan)(&wifi_table_length);
        for (n = 0; n < wifi_table_length; n++) {
            if (!strcmp(ssid, wifi_table->ssid)) break;
            wifi_table++;
        }
    }


    if ((*w->connect)(wifi_table, password)) {
        printf("success\n");
    }
    else {
        printf("failed\n");
    }

    return 0;
}
