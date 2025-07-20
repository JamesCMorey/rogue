#include "world.h"
#include "evloop.h"
#include "vfx.h"
#include "log.h"
#include "geometry.h"
#include "scene.h"
#include "player.h"
#include "gamestate.h"
#include "scene_render.h"
#include <stdlib.h>

static Coord cnk2idx(Coord cnk) {
	return coord(cnk.y + WORLD_HEIGHT/2, cnk.x + WORLD_WIDTH/2);
}

// ------ World initialization ------

static void place_door(Room *r, Direction dir) {
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

static void place_doors(Room *r) {
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

static void sector_init(Sector *s) {
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

static void chunk_init(WorldData *world, int cnk_y, int cnk_x) {
	Coord idx = cnk2idx(coord(cnk_y, cnk_x));
	Chunk *cnk = &world->chunks[idx.y][idx.x];

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

void world_init(WorldData *world) {
	log_fmt(LOG_GEN, "world_init(): Initializing world.\n");

	for (int y = 0; y < WORLD_HEIGHT; ++y) {
		for (int x = 0; x < WORLD_WIDTH; ++x) {
			world->chunks[y][x].initialized = false;
		}
	}

	/* Generate rooms */
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			chunk_init(world, y, x);
		}
	}
}

void chunk_auto_init(GameState *gs) {
	WorldData *world = &gs->dungeon;
	if (!pl_changed_cnk(&gs->player))
		return;

	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			// TODO: Fix the chunk coord and chunk array coord disconnect/confusion
			Coord cnk = coord_add(pl_cnk(&gs->player), coord(y, x));
			Coord idx = cnk2idx(cnk);

			if (world->chunks[idx.y][idx.x].initialized)
				continue;

			chunk_init(world, cnk.y, cnk.x);
		}
	}
}

// ------ Utilities ------

Chunk *world_cnk(WorldData *world, Coord cnk) {
	Coord idx = cnk2idx(cnk);
	return &world->chunks[idx.y][idx.x];
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

void chunk_reset_random(Chunk *cnk) {
	cnk->rng_itr = 0;
}

void world_ctx_teardown(WorldContext *context) {
	WorldContext *ctx = context;

	log_fmt(LOG_GEN, "world_ctx_teardown(): Leaving world loop.\n");

	if (ctx && ctx->gs)
		free(ctx->gs);

	free(ctx);
}

// ------ Event Loop Entry ------

LogicFrameAction world_update(void *context) {
	WorldContext *ctx = context;

	char c = get_input();
	if (c == 'q')
		return LFRAME_EXIT;

	PlayerAction act = pl_action(c);
	scn_update(ctx->gs, act);

	return LFRAME_NOP;
}

void world_exit(void *context) {
	WorldContext *ctx = context;
	ctx->exit_fn(context);
}

void world_enter(void *context) {
	WorldContext *ctx = context;

	log_fmt(LOG_GEN, "world_enter(): Entering world loop.\n");

	eventloop_enter(ctx, world_render, world_update, world_exit);
}
