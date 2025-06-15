#include <ncurses.h>
#include "vfx.h"
#include "world.h"
#include "evloop.h"
#include "menu.h"

int main() {
	vfx_init();

	main_menu_create();
	eventloop_run();
	
	vfx_teardown();
	return 0;
}
