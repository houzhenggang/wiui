#include "dialog.h"

static void sig_handler(int signo)
{
    int res = 1;
	exit(res);
}

int main() {
    int s_scroll = 0;
    int ret;
    setlocale(LC_ALL, "");
    signal(SIGINT, sig_handler);
    
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
    return 0;
}
