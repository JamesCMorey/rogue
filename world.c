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
		r->onscreen = draw_rect(camy(r->world_y), camx(r->world_x), r->height, r->width);
	}

	draw_rect(maxy/2, maxx/2, 1, 2);
	mvprintw(maxy - 1, 0, "y:%d, x:%d", world.player.y, world.player.x);
	// int cnt = 0;
	// for (int i = 0; i < world.room_num; ++i) {
	// 	if (world.rooms[i].onscreen)
	// 		++cnt;
	// }
	// mvprintw(maxy - 2, 0, "Checking %d rooms for collision", cnt);
}

void move_player(int y, int x) {
	int up, down, right, left;
	up = down = right = left = 1;

	Player *p = &world.player;
	Room *r;
	for (int i = 0; i < world.room_num; ++i) {
		r = &world.rooms[i];
		/* Check if on screen and in room */
		if (r->onscreen
				&& (r->world_x < p->x && p->x < r->width + r->world_x)
				&& (r->world_y < p->y && p->y < r->height + r->world_y)) {
			/* Check if next to wall */
			if (p->y + 1 == r->world_y + r->height - 1) down = 0;
			if (p->y - 1 == r->world_y) up = 0;
			if (p->x - 1 == r->world_x) left = 0;
			if (p->x + 1 == r->world_x + r->width - 2) right = 0;
		}
	}

	/* Move if allowed */
	if ((y > 0 && down) || (y < 0 && up)) p->y += y;
	if ((x > 0 && right) || (x < 0 && left)) p->x += x;
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
	r->onscreen = 1;

	r = &world.rooms[1];
	r->world_x = -10;
	r->world_y = -10;
	r->width = 10;
	r->height = 20;
	r->onscreen = 1;
}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	world_gen();
	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
