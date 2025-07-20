#include <stdlib.h>
#include <time.h>
#include "vfx.h"
#include "world.h"
#include "evloop.h"
#include "menu.h"
#include "log.h"


int main() {
	log_init();
	vfx_init();
	srand(time(NULL));

	main_menu_create(NULL);
	eventloop_run();

	vfx_teardown();
	log_teardown();
	return 0;
}
