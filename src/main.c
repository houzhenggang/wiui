#include "dialog.h"
#include "wiui.h"
#include<pthread.h> 
#include <syslog.h>
static void sig_handler(int signo)
{
    int res = 1;
	exit(res);
}
void *scan_func(void *arg)  
{  
    struct wifi_describe* wifi_table;
    wiui *w;    
    int n;
    int wifi_table_length = 3;
    int max_choice;
    w = get_wifi();
    wifi_table = (*w->scan)(&wifi_table_length);
    sleep(5);
    item_reset(); 

    for (n = 0; n < wifi_table_length;n++) {
        item_make("%s", wifi_table->ssid);
        syslog(LOG_INFO, "main: - %s %s \n", wifi_table->ssid, wifi_table->security);
        item_set_tag('m');
        wifi_table++;
    }
    syslog(LOG_INFO, "main: -wifi_table_length:%d \n", wifi_table_length);
    max_choice = MIN(get_menu_height(), item_count());
	/* Print the menu */
	for (n = 0; n < max_choice; n++) {
		print_item(n, n, 0);
	}
    fresh_menu();

    free(w);
    free(wifi_table);
    return ((void *)0);  
}  
int main() {
    int s_scroll = 0;
    int ret;
    pthread_t scan_tid;  

    setlocale(LC_ALL, "");
    signal(SIGINT, sig_handler);
    
    pthread_create(&scan_tid, NULL, scan_func, NULL);

     
	if (init_dialog(NULL)) {
		fprintf(stderr, "Your display is too small to run wiui!\n");
		fprintf(stderr, "It must be at least 19 lines by 80 columns.\n");
		return 1;
	}

    item_reset();
    item_make("Scan wifi,Please wait ....");
    item_set_tag('m');


    dialog_menu("WIUI visible","Please set up your wifi !",NULL,&s_scroll);

    end_dialog(0, 0);

    return 0;
}
