#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "vfx.h"
#include "world.h"
#include "evloop.h"
#include "menu.h"


int main() {
	vfx_init();
	srand(time(NULL));

	main_menu_create(NULL);
	eventloop_run();
	
	vfx_teardown();
	return 0;
}
