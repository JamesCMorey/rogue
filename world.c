#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include "log.h"
#include "geometry.h"
#include "scene.h"
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

WorldData world;

/* The following applies to all chunk functions (render_chunk, gen_chunk, etc.)
 *
 * Params: The parameters x and y will be the x and y in the world, so (0, 0)
 * corresponds to the chunk at (0, 0) in the world
 * */

void render_chunkmap() { // TODO: Make this actually draw a chunkmap
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			Coord w = coord_add(chunk_offset(-1, -1),
			          coord_add(sector_offset(y, x),
			                    coord(1, 1)));
			draw_rect(camy(w.y), camx(w.x), SECTOR_HEIGHT, SECTOR_WIDTH);
		}
	}
}

void render_doors(int r_world_y, int r_world_x, Room *r) {
	if (!r->onscreen)
		return;

	for (int k = 0; k < 4; ++k) {
		Coord *d = &r->doors[k];

		if (k % 2 == 0) {
			/* TODO: partial displays */
			mvprintw(camy(r_world_y + d->y), camx(r_world_x + d->x), "+++"); 
		} else {
			mvprintw(camy(r_world_y + d->y), camx(r_world_x + d->x), "+");
			mvprintw(camy(r_world_y + d->y + 1), camx(r_world_x + d->x), "+");
		}
	}
}

void render_chunk(int cy, int cx) {
	Chunk *cnk = &world.chunks[cy + WORLD_HEIGHT/2][cx + WORLD_WIDTH/2];

	for (int sy = 0; sy < 3; ++sy) {
		for (int sx = 0; sx < 3; ++sx) {
			Room *r = &cnk->sectors[sy][sx].r;
			Coord w = world_offset(cy, cx, sy, sx, r->y, r->x);

			// int r_world_y = world_y(cy, sy, r->y);
			// int r_world_x = world_x(cx, sx, r->x);
			r->onscreen = draw_rect(camy(w.y), camx(w.x),
			                        r->height, r->width);

			render_doors(w.y, w.x, r);
		}
	}
}

void render_world(void *context) {
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			render_chunk(y, x);
		}
	}

	//render_chunkmap();
	draw_rect(maxy/2, maxx/2, 2, 3); // visual square; logical nightmare
	mvprintw(maxy - 1, 0, "y:%d, x:%d", world.player.y, world.player.x);
}

void move_player(int y, int x) {
	int up, down, right, left;
	up = down = right = left = 1;

	Player *p = &world.player;
	// Room *r;
	// for (int i = 0; i < world.room_num; ++i) {
	// 	r = &world.chunks[50][50];
	// 	/* Check if on screen and in room */
	// 	if (r->onscreen
	// 			&& (r->world_x < p->x && p->x < r->width + r->world_x)
	// 			&& (r->world_y < p->y && p->y < r->height + r->world_y)) {
	// 		/* Check if next to wall */
	// 		if (p->y + 1 == r->world_y + r->height - 1) down = 0;
	// 		if (p->y - 1 == r->world_y) up = 0;
	// 		if (p->x - 1 == r->world_x) left = 0;
	// 		if (p->x + 1 == r->world_x + r->width - 2) right = 0;
	// 	}
	// }
	//
	// /* Move if allowed */
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

void place_doors(Room *r) {
	for (int k = 0; k < 4; ++k) {
		//if (random() % 3) {
		switch(k) {
			case 0: // above
				r->doors[k].x = random()%(r->width - 3) + 1;
				r->doors[k].y = 0;
				break;
			case 1: // right
				r->doors[k].x = r->width;
				r->doors[k].y = random()%(r->height - 2) + 1;
				break;
			case 2: // below
				r->doors[k].x = random()%(r->width - 3) + 1;
				r->doors[k].y = r->height;
				break;
			case 3: // left
				r->doors[k].x = 0;
				r->doors[k].y = random()%(r->height - 2) + 1;
				break;
		}
		//}
	}
}

void sector_init(Sector *s) {
	Room *r = &s->r;
	s->hasroom = true;

	r->width = random() % 15 + 10;
	r->height = random() % 5 + 10;
	r->y = random()%(SECTOR_HEIGHT - r->height); 
	r->x = random()%(SECTOR_WIDTH - r->width);

	r->onscreen = 1;
	place_doors(r);
}

void chunk_init(int cnk_y, int cnk_x) {
	Chunk *cnk = &world.chunks[cnk_y + WORLD_HEIGHT/2][cnk_x + WORLD_WIDTH/2];

	/* TODO: Make this switch out as player moves */
	scn.chunks[cnk_y + 1][cnk_x + 1] = cnk;

	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			sector_init(&cnk->sectors[y][x]);
		}
	}
}

void world_init() {
	/* Generate rooms */
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			chunk_init(y, x);
		} 
	}
	scn_init();
	// join_sectors(0, 0, 0, 1); // just hardcode/try top left sectors for now

	for (int i = 0; i < CHUNK_HEIGHT*3; ++i) {
		wlog(scn.tm[i], CHUNK_WIDTH*3, sizeof(char));
		plog("\n");
	}
}

void doom_world(void *context) {
	return;
}

void enter_world(void *context) {
	world.player.x = 0;
	world.player.y = 0;

	world_init();
	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}
