#include "scene.h"
#include "geometry.h"
#include "world.h"
#include "log.h"
#include "player.h"
#include "gamestate.h"
#include <string.h>
#include <stdlib.h>

// ------ Tile Utilities ------

static char *tile_at(Scene *scn, Coord c) { return &scn->tm[c.y][c.x]; }
static bool tile_clear(char *t) { return *t == CO_EMPTY || *t == CO_HALL; }

// ------  Sector/Room Access ------

static Sector *scn_sector(Scene *scn, Coord cnk, Coord sec) {
	return &scn->chunks[cnk.y][cnk.x]->sectors[sec.y][sec.x];
}

static Coord scn_point(Coord cnk, Coord sec, Coord pnt) {
	return coord_add(chunk_offset(cnk.y, cnk.x),
	       coord_add(sector_offset(sec.y, sec.x),
	                 pnt));
}

static Coord scn_door_coord(Scene *scn, Coord cnk, Coord sec, Direction dir) {
	Sector *s = scn_sector(scn, cnk, sec);

	// find the scn coords for the pnt of the door pnt coords = (room  + door)
	Coord pnt = scn_point(cnk, sec, coord_add(coord(s->r.y, s->r.x), s->r.doors[dir]));

	switch (dir) {
		case UP:
			pnt = coord_add(pnt, coord(-1, 0));
			break;
		case RIGHT:
			pnt = coord_add(pnt, coord(0, 1));
			break;
		case DOWN:
			pnt = coord_add(pnt, coord(1, 0));
			break;
		case LEFT:
			pnt = coord_add(pnt, coord(0, -1));
			break;
		default:
			break;
	}

	return pnt;
}

static Direction random_door(Scene *scn, Coord cnk, Room *r) {
	int rand = chunk_random(scn->chunks[cnk.y][cnk.x]);
	int door_chosen = (rand%r->door_num) + 1; // +1 so it's 1-4 and not 0-3

	int doors_seen = 0;
	int idx = -1;
	while (doors_seen < door_chosen) {
		++idx;
		if (r->valid_doors[idx]) // put here so it leaves loop if room is found
			++doors_seen;
	}

	return idx;
}

// ------ Hall Generation ------

/* Connect two sectors in scn by choosing a random door on each
 * sector's room and then digging a hall between them. The sectors can belong
 * to different chunks.
 *
 * Params:
 *  cnk1, cnk2: coords of each chunk in scn
 *	s1, s2: coords of sector within their respective chunk
 * */
void join_sectors(Scene *scn, Coord cnk1, Coord s1, Coord cnk2, Coord s2) {
	Sector *s = scn_sector(scn, cnk1, s1);
	Coord cur = scn_door_coord(scn, cnk1, s1, random_door(scn, cnk1, &s->r));

	s = scn_sector(scn, cnk2, s2);
	Coord tar = scn_door_coord(scn, cnk2, s2, random_door(scn, cnk2, &s->r));

	// Compute the distance of the y- and x-components between the doors
	int diff[2] = {coord_sub(tar, cur).y, coord_sub(tar, cur).x};
	int mvd[2] = { 0 }; // Store distance traveled from source door

	int prio = 0; // Directional priority: 0 = y, 1 = x

	while (mvd[0] != diff[0] || mvd[1] != diff[1]) {
		/* dir[i] is unit direction for the i axis. This is either -1 or 1.
		 * For example: dir[0] = -1 means move up; dir[1] = 1 means move right.
		 *
		 * The equation subtracts movement made along the axis so that the direction
		 * is relative to the current position and not a set point.
		 * */
		int r1 = chunk_random(scn->chunks[cnk1.y][cnk1.x])%2 ? -1 : 1;
		int r2 = chunk_random(scn->chunks[cnk2.y][cnk2.x])%2 ? -1 : 1;
		int dir[2] = {
			diff[0] == mvd[0] ? r1 : (diff[0] - mvd[0]) / abs(diff[0] - mvd[0]),
			diff[1] == mvd[1] ? r2 : (diff[1] - mvd[1]) / abs(diff[1] - mvd[1])
		};

		// next step along each axis
		Coord next[2] = {
			coord_add(cur, coord(dir[0], 0)),
			coord_add(cur, coord(0, dir[1]))
		};

		char *tile[2] = { tile_at(scn, next[0]), tile_at(scn, next[1]) };

		/* Try to move in current priority axis under the following conditions:
		 * 1. the target hasn't been reached yet
		 * 2. the next tile along that axis is open
		 * 3. the step being taken along the axis will bring us closer to the target
		 * 4. or, in lieu of 3., the other priority axis is currently stuck
		 * */
		if ((mvd[prio] != diff[prio] || mvd[!prio] != diff[!prio])
			  && tile_clear(tile[prio])
		    && (abs(diff[prio] - (mvd[prio] + dir[prio])) < abs(diff[prio] - mvd[prio])
		    || !tile_clear(tile[!prio]))) {
			int tmp[2] = { 0 };
			do {
				*tile[prio] = CO_HALL;
				mvd[prio] += dir[prio];
				cur = next[prio];

				tmp[0] = 0; tmp[1] = 0;
				tmp[prio] = dir[prio];
				next[prio] = coord_add(cur, coord(tmp[0], tmp[1]));
				tile[prio] = tile_at(scn, next[prio]);

				// prepare for escape condition (target reached or other axis is free)
				tmp[0] = 0; tmp[1] = 0;
				tmp[!prio] = dir[!prio];
			} while (!(mvd[prio] == diff[prio] && mvd[!prio] == diff[!prio])
			          && !tile_clear(tile_at(scn, coord_add(cur, coord(tmp[0], tmp[1])))));
		}

		// alternate axis
		prio = !prio;
	}
}

/*
 * TODO
 * 1. Join across chunks
 * 2. Make pathing more sane
 * 3. Make all doors get used instead of random selection
 * */
void gen_halls(Scene *scn, int cy, int cx) {
	/* reset rng array of both chunk to start of array so each time this function
	 * is called, it creates the same 'random' output.
	 * */
	chunk_reset_random(scn->chunks[cy][cx]);

	// modifiable copy
	int tmp_cnt[3][3];
	for (int y = 0; y < 3; ++y) {
		for(int x = 0; x < 3; ++x) {
			tmp_cnt[y][x] = scn->chunks[cy][cx]->sector_door_counts[y][x];
		}
	}

	// for each sector
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			// for each door
			while (tmp_cnt[y][x] > 0) {

				int ry, rx;
				ry = rx = 0;
				int rand = chunk_random(scn->chunks[cy][cx]);
				if (y == 2 && x == 2) {
					rx = rand%3;
					ry = rand%3;
				}
				else {
					rx = rand%(3-x) + x;
					ry = rand%(3-y) + y;
				}

				int abort = 0;
				while (abort < 10 && ((rx == x && ry == y)
				       || (tmp_cnt[ry][rx] == 0 && !(y == 2 && x == 2)))) {
					rand = chunk_random(scn->chunks[cy][cx]);
					rx = rand%(3-x) + x;
					ry = rand%(3-y) + y;
					++abort; // prevent infinite loop
				}

				join_sectors(scn, coord(cy, cx), coord(y, x), coord(cy, cx), coord(ry, rx));
				--tmp_cnt[y][x];
				--tmp_cnt[ry][rx];
			}
		}
	}
}

// ------- Scene Loading ------

/* Load a sector from the chunk references into the tilemap.
 *
 * Params:
 *  cnk: coords for the chunk in scn.chunks
 *  sec: coords for sector within specified chunk
 *
 * This function calculates the absolute tilemap position of the sector's
 * room before rendering its walls and doors into the scn.tm tilemap.
 * (The responsibilities of this function may expand in the future)
 * */
void scn_load_sector(Scene *scn, Coord cnk, Coord sec) {
	Sector *s = scn_sector(scn, cnk, sec);

	// Top-left corner of room in tilemap
	Coord r = coord_add(chunk_offset(cnk.y, cnk.x),
	          coord_add(sector_offset(sec.y, sec.x),
	                    coord(s->r.y, s->r.x)));

	// Draw horizontal walls (top and bottom)
  // see draw_rect() for -1 explanation
	for (int x = 0; x < s->r.width; ++x) {
		scn->tm[r.y][r.x + x] = CO_WALL;                      // top
		scn->tm[r.y + s->r.height - 1][r.x + x] = CO_WALL;    // bottom
	}

	// Draw vertical walls (left and right)
	for (int y = 0; y < s->r.height; ++y) {
		scn->tm[r.y + y][r.x] = CO_WALL;                      // left
		scn->tm[r.y + y][r.x + s->r.width - 1] = CO_WALL;     // right
	}

	// Place doors on walls
	for (int k = 0; k < 4; ++k) {
		if (!s->r.valid_doors[k])
			continue;

		Coord d = coord_add(r, s->r.doors[k]);

		// Horizontal doors: 0 (up) and 2 (down)
		if (k % 2 == 0) {
			scn->tm[d.y][d.x] =
			scn->tm[d.y][d.x + 1] =
			scn->tm[d.y][d.x + 2] = CO_CLOSED_DOOR;
		}
		// Vertical doors: 1 (right) and 3 (left)
		else {
			scn->tm[d.y][d.x] =
			scn->tm[d.y + 1][d.x] = CO_CLOSED_DOOR;
		}
	}
}

/* cy and cx are indexes into the chunk references */
void scn_load_chunk(Scene *scn, Chunk *c, int cy, int cx) {
	// store reference
	scn->chunks[cy][cx] = c;

	// Clear chunk area in tilemap before drawing the chunk in it
	for (int y = 0; y < CHUNK_HEIGHT; ++y) {
		memset(&scn->tm[cy*CHUNK_HEIGHT + y][cx*CHUNK_WIDTH], CO_EMPTY, CHUNK_WIDTH);
	}

	// Load sectors of chunk into tilemap
	for (int sy = 0; sy < 3; ++sy) {
		for (int sx = 0; sx < 3; ++sx) {
			scn_load_sector(scn,
			                coord(cy, cx),
			                coord(sy, sx));
		}
	}

	// Draw halls in tilemap
	gen_halls(scn, cy, cx);
}

void scn_load(GameState *gs) {
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			// load player and surrounding chunks
			Coord cnk = coord_add(pl_cnk(&gs->player), coord(y, x));
			if (world_coord_valid(&gs->dungeon, cnk)) {
				Chunk *c = world_cnk(&gs->dungeon, cnk);
				scn_load_chunk(&gs->scene, c, y + 1, x + 1);
			}
		}
	}
}

void scn_init(GameState *gs) {
	log_fmt(LOG_GEN, "scn_init(): Initializing scene.\n");
	scn_load(gs);
}

// ------ Scene Logic ------

void scn_move(GameState *gs, PlayerAction act) {
	move_player(gs, act.move);

	if (pl_changed_cnk(&gs->player)) {
		// Initialize new chunks and load them into scn
		chunk_init_around(&gs->dungeon, pl_cnk(&gs->player));
		scn_load(gs);
	}
}

void scn_update(GameState *gs, PlayerAction act) {
	switch (act.type) {
		case PA_MOVE:
			scn_move(gs, act);
			break;
		default:
			break;
	}
}
