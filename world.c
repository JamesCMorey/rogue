#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include <stdlib.h>
#include <ncurses.h>

WorldData world;

void render_world(void *context) {

	Chunk *cnk = &world.chunks[50][50];
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			Room *r = &cnk->rooms[y][x];
			r->onscreen = draw_rect(camy(r->world_y), camx(r->world_x), r->height, r->width);

			if (r->onscreen) {
				Door *d;
				for (int k = 0; k < 4; ++k) {
					d = &r->doors[k];
					if (k % 2 == 0) {
						mvprintw(camy(d->y), camx(d->x), "+++"); /* todo: partial displays */
					} else {
						mvprintw(camy(d->y), camx(d->x), "+");
						mvprintw(camy(d->y + 1), camx(d->x), "+");
					}
				}
			}
		}
	}

	draw_rect(maxy/2, maxx/2, 1, 2);
	mvprintw(maxy - 1, 0, "y:%d, x:%d", world.player.y, world.player.x);
}

void move_player(int y, int x) {
	int up, down, right, left;
	up = down = right = left = 1;

	Player *p = &world.player;
	Room *r;
	for (int i = 0; i < world.room_num; ++i) {
		r = &world.chunks[50][50];
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
	// if ((y > 0 && down) || (y < 0 && up)) p->y += y;
	// if ((x > 0 && right) || (x < 0 && left)) p->x += x;
	p->y += y;
	p->x += x;
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

void place_doors(Room *r) {
	for (int k = 0; k < 4; ++k) {
		//if (random() % 3) {
		switch(k) {
			case 0: // above
				r->doors[k].x = r->world_x + random()%(r->width - 3) + 1;
				r->doors[k].y = r->world_y;
				break;
			case 1: // right
				r->doors[k].x = r->world_x + r->width;
				r->doors[k].y = r->world_y + random()%(r->height - 2) + 1;
				break;
			case 2: // below
				r->doors[k].x = r->world_x + random()%(r->width - 3) + 1;
				r->doors[k].y = r->world_y + r->height;
				break;
			case 3: // left
				r->doors[k].x = r->world_x;
				r->doors[k].y = r->world_y + random()%(r->height - 2) + 1;
				break;
		}
		//}
	}
}

void gen_chunk(int cnk_y, int cnk_x) {
	Chunk *cnk = &world.chunks[cnk_y][cnk_x];

	Room *r;
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			r = &cnk->rooms[y][x];
			r->width = random() % 15 + 10;
			r->height = random() % 5 + 10;
			// offsets take the chunk or sector and convert them to world coords
			// TODO: Fix the magic '51' that corresponds to the index in the world.chunks array		//            chunk offset           sector offset      random spot in sector
			r->world_y = ((cnk_y - 51)*CHUNK_HEIGHT) + (SECTOR_HEIGHT*y+1) + random()%(SECTOR_HEIGHT - r->height); 
				//CHUNK_HEIGHT*y+1 + random()%(CHUNK_HEIGHT - r->height);
			r->world_x = ((cnk_x - 51)*CHUNK_WIDTH) + (SECTOR_WIDTH*x+1) + random()%(SECTOR_WIDTH - r->width);
				//CHUNK_WIDTH*x+1 + random()%(CHUNK_WIDTH - r->width);

			r->onscreen = 1;
			place_doors(r);
		}
	}
	cnk->room_num = 9;
}

void world_gen() {
	/* Generate rooms */
	world.room_num = 9;
	for (int i = 0; i < 9; ++i) {
		gen_chunk(50 + i/3 - 1, 50 + i%3 - 1); // generate 3x3 around 0,0 (50, 50)
	}
}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	world_gen();
	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
