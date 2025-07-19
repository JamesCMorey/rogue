#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include "log.h"
#include "geometry.h"
#include "scene.h"
#include "player.h"
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

WorldData world;

void render_chunkmap() { // TODO: Make this actually draw a chunkmap
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Coord tl = world_offset(coord(y, x), coord(0, 0), coord(0, 0));
			Coord cam = cam_coord(tl);
			draw_rect(cam.y, cam.x, CHUNK_HEIGHT, CHUNK_WIDTH);
		}
	}
}

void render_doors(int r_world_y, int r_world_x, Room *r) {
	if (!r->onscreen)
		return;

	for (int i = 0; i < 4; ++i) {
		if (!r->valid_doors[i])
			continue;

		Coord *d = &r->doors[i];

		Coord cam = cam_coord(coord(r_world_y + d->y, r_world_x + d->x));
		if (i % 2 == 0) {
			/* TODO: partial displays */
			mvprintw(cam.y, cam.x, "+++");
		} else {
			mvprintw(cam.y, cam.x, "+");
			mvprintw(cam.y + 1, cam.x, "+");
		}
	}
}

void render_chunk(int cy, int cx) {
	Chunk *cnk = &world.chunks[cy + WORLD_HEIGHT/2][cx + WORLD_WIDTH/2];

	for (int sy = 0; sy < 3; ++sy) {
		for (int sx = 0; sx < 3; ++sx) {
			Room *r = &cnk->sectors[sy][sx].r;
			Coord w = world_offset(coord(cy, cx), coord(sy, sx), coord(r->y, r->x));

			Coord cam = cam_coord(coord(w.y, w.x));
			r->onscreen = draw_rect(cam.y, cam.x,
			                        r->height, r->width);

			render_doors(w.y, w.x, r);
		}
	}
}

void render_world(void *context) {
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Coord pos = abs2cnk(pl_get_abs());
			render_chunk(pos.y + y, pos.x + x);
		}
	}

	render_chunkmap();
	draw_rect(maxy/2, maxx/2, 2, 3); // visual square; logical nightmare
	mvprintw(maxy - 1, 0, "y:%d, x:%d", pl_get_abs().y, pl_get_abs().x);
}

void place_door(Room *r, Direction dir) {
	switch(dir) {
		case UP:
			// +1 to make sure it's not on left edge
			// -3 for length of door -1 for right edge
			r->doors[dir].x = random()%(r->width - 3 - 1) + 1;
			r->doors[dir].y = 0;
			break;
		case RIGHT:
			r->doors[dir].x = r->width - 1; // see draw_rect() for -1 explanation
			// +1 to make sure it's not on upper edge
			// -2 for length of door -1 for bottom edge
			r->doors[dir].y = random()%(r->height - 2 - 1) + 1;
			break;
		case DOWN:
			r->doors[dir].x = random()%(r->width - 3 - 1) + 1;
			r->doors[dir].y = r->height - 1; // see draw_rect() for -1 explanation
			break;
		case LEFT:
			r->doors[dir].x = 0;
			r->doors[dir].y = random()%(r->height - 2 - 1) + 1;
			break;
		default:
			break;
	}

	++r->door_num;
	r->valid_doors[dir] = true;
}

void place_doors(Room *r) {
	// initialize to proper values
	r->door_num = 0;
	for (Direction dir = 0; dir < DIR_COUNT; ++dir) {
		r->valid_doors[dir] = false;
	}

	// guarantee at least one door on each room
	Direction used = random()%DIR_COUNT;
	place_door(r, used);

	for (Direction dir = 0; dir < DIR_COUNT; ++dir) {
		if (random()%3==0 && dir != used) { // 1/3 chance
			place_door(r, dir);
		}
	}
}

void sector_init(Sector *s) {
	Room *r = &s->r;
	s->hasroom = true;

	r->width = random() % 15 + 10;
	r->height = random() % 5 + 10;

	/* Room size should be reduced by one because if there is `room width` amount
	 * of space left, there is still space for a room. Take one away and you can
	 * no longer fit a room in the space left over.
	 *
	 * The -4+2 and -6+3 create a buffer zone so rooms cannot be generated at the
	 * edge of the sector. This is necessary because otherwise it can cause
	 * out-of-bounds pathing in join_sectors() when it tries to go around the room.
	 * */
	r->y = random()%(SECTOR_HEIGHT - (r->height - 1) - 4) + 2;
	r->x = random()%(SECTOR_WIDTH - (r->width - 1) - 6) + 3;

	r->onscreen = 1;
	place_doors(r);
}

void chunk_init(int cnk_y, int cnk_x) {
	Coord idx = cnk2idx(coord(cnk_y, cnk_x));
	Chunk *cnk = &world.chunks[idx.y][idx.x];

	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			sector_init(&cnk->sectors[y][x]);
			cnk->sector_door_counts[y][x] = cnk->sectors[y][x].r.door_num;
		}
	}

	cnk->initialized = true;
	cnk->rng_full = false;
	cnk->rng_itr = 0;
}

void chunk_auto_init() {
	if (!pl_get_changed_cnk())
		return;

	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			// TODO: Fix the chunk coord and chunk array coord disconnect/confusion
			Coord cnk = coord_add(pl_get_cnk(), coord(y, x));
			Coord idx = cnk2idx(cnk);

			if (world.chunks[idx.y][idx.x].initialized)
				continue;

			chunk_init(cnk.y, cnk.x);
		}
	}
}

void world_init() {
	for (int y = 0; y < WORLD_HEIGHT; ++y) {
		for (int x = 0; x < WORLD_WIDTH; ++x) {
			world.chunks[y][x].initialized = false;
		}
	}

	/* Generate rooms */
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			chunk_init(y, x);
		}
	}

	scn_init();
	pl_set_abs(coord(0, 0));
}

LogicFrameAction simulate_world(void *context) {
	char c = get_input();
	if (c == 'q')
		return LFRAME_EXIT;

	handle_movement(c);
	chunk_auto_init();

	PlayerAction act;
	act.type = PL_MOVE;
	scn_update(act);

	return LFRAME_NOP;
}

void doom_world(void *context) {
	return;
}

void enter_world(void *context) {
	world_init();
	eventloop_enter(NULL, render_world, simulate_world, doom_world);
}

void chunk_reset_random(Chunk *cnk) {
	cnk->rng_itr = 0;
}

int chunk_random(Chunk *cnk) {
	int r;

	if (cnk->rng_full) {
		r = cnk->rng[cnk->rng_itr++];
	} else {
		r = random();
		cnk->rng[cnk->rng_itr++] = r;
	}

	if (cnk->rng_itr >= CNK_RNG_CNT) {
		cnk->rng_itr = 0;
		cnk->rng_full = true;
	}

	return r;
}

Chunk *world_get_cnk(Coord cnk) {
	return &world.chunks[cnk.y][cnk.x];
}
