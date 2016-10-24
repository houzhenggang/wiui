#include "dialog.h"
#include "wiui.h"

static void sig_handler(int signo)
{
    int res = 1;
	exit(res);
}

int main() {
    int s_scroll = 0;
    int ret;
    struct wifi_describe* wifi_table = (struct wifi_describe*) calloc(1, sizeof(struct wifi_describe));
    wiui *w;
    setlocale(LC_ALL, "");
    signal(SIGINT, sig_handler);
    
    w = get_wifi();
    w->scan(wifi_table);
     
	if (init_dialog(NULL)) {
		fprintf(stderr, "Your display is too small to run wiui!\n");
		fprintf(stderr, "It must be at least 19 lines by 80 columns.\n");
		return 1;
	}

    item_reset();
    item_make("Helloworld1");
    item_set_tag('m');
    item_make("Helloworld2");
    item_set_tag('m');
    item_make("Helloworld3");
    item_set_tag('m');


    dialog_menu("WIUI visible","Please set up your wifi !",NULL,&s_scroll);

	do {
		ret = getch();
	} while (ret == KEY_ESC);
    end_dialog(0, 0);

    free(w);
    return 0;
}
