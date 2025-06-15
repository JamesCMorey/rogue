#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include <stdlib.h>
#include <ncurses.h>

WorldData world;

void render_world(void *context) {
	Room *r;
	for (int i = 0; i < world.room_num; ++i) {
		r = &world.rooms[i];
		draw_rect(camy(r->world_y), camx(r->world_x), r->height, r->width);
	}

	draw_rect(maxy/2, maxx/2, 1, 2);
	mvprintw(maxy - 1, 0, "y:%d, x:%d", world.player.y, world.player.x);
}

void move_player(int y, int x) {
	Player *p = &world.player;
	if (p->y + y < 20) p->y += y;
	if (p->x + x < 20) p->x += x;
}

void handle_movement(char c) {
	switch(c) {
		case 'k': case 'w': move_player(-1, 0); break;
		case 'h': case 'a': move_player(0, -1); break;
		case 'j': case 's': move_player(1, 0); break;
		case 'l': case 'd': move_player(0, 1); break;

		case 'y': move_player(-1, -1); break;
		case 'u': move_player(-1, 1); break;
		case 'b': move_player(1, -1); break;
		case 'n': move_player(1, 1); break;
		default: break;
	}
}

LogicFrameAction simulate_world(void *context) {
	char c = get_input();
	if (c == 'q')
		return LFRAME_EXIT;

	handle_movement(c);
	
	return LFRAME_NOP;
}

void doom_world(void *context) {
	return;
}

void world_gen() {
	world.room_num = 2;

	Room *r = &world.rooms[0];
	r->world_x = 12;
	r->world_y = 12;
	r->width = 20;
	r->height = 10;

	r = &world.rooms[1];
	r->world_x = -10;
	r->world_y = -10;
	r->width = 10;
	r->height = 20;
}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	world_gen();
	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
