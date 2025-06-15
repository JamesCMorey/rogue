#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include <stdlib.h>


WorldData world;

// void handle_input(bool *running) {
// 		char ch = get_input();
// 		switch(ch) {
// 			case 'q': *running = false; break;
// 			case 'k':
// 			case 'w': world.player.y -= 1; break;
// 			case 'h':
// 			case 'a': world.player.x -= 1; break;
// 			case 'j':
// 			case 's': world.player.y += 1; break;
// 			case 'l':
// 			case 'd': world.player.x += 1; break;
// 			default: break;
// 		}
// }

void render_world(void *context) {
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c)
			draw_rect(camy(20 * r), camx(40 * c), 20, 40);
	}

	draw_rect(maxy/2, maxx/2, 1, 2);
}

LogicFrameAction simulate_world(void *context) {
	return LFRAME_NOP;
}

void doom_world(void *context) {

}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
