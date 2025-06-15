#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include <stdlib.h>


WorldData world;


void render_world(void *context) {
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c)
			draw_rect(camy(20 * r), camx(40 * c), 20, 40);
	}

	draw_rect(maxy/2, maxx/2, 1, 2);
}

void move_player(int y, int x) {
	Player *p = &world.player;
	if (p->y + y < 20) p->y += y;
	if (p->x + x < 20) p->x += x;
}

LogicFrameAction simulate_world(void *context) {
	char ch = get_input();
	switch(ch) {
		case 'q': return LFRAME_EXIT; break;
		case 'k':
		case 'w': move_player(1, 0); break;
		case 'h':
		case 'a': move_player(0, -1); break;
		case 'j':
		case 's': move_player(-1, 0); break;
		case 'l':
		case 'd': move_player(0, 1); break;

		case 'y': move_player(-1, -1); break;
		case 'u': move_player(-1, 1); break;
		case 'b': move_player(1, -1); break;
		case 'n': move_player(1, 1); break;
		default: break;
	}
	return LFRAME_NOP;
}

void doom_world(void *context) {
	return;
}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
