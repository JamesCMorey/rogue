#include "scene.h"
#include "geometry.h"
#include "world.h"
#include "log.h"
#include <string.h>
#include <stdlib.h>

Scene scn;

Sector *scn_sector(Coord cnk, Coord sec) {
	return &scn.chunks[cnk.y][cnk.x]->sectors[sec.y][sec.x];
}

Coord scn_point(Coord cnk, Coord sec, Coord pnt) {
	return coord_add(chunk_offset(cnk.y, cnk.x),
	       coord_add(sector_offset(sec.y, sec.x),
	                 pnt));
}

Coord scn_door_coord(Coord cnk, Coord sec, Direction dir) {
	Sector *s = scn_sector(cnk, sec);

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
	}

	return pnt;
}

char *tile_at(Coord c) {
	return &scn.tm[c.y][c.x];
}

bool tile_clear(char *t) {
	return *t == CO_EMPTY || *t == CO_HALL;
}

Direction random_door(Room *r) {
	int door_chosen = (random()%r->door_num) + 1; // +1 so it's 1-4 and not 0-3

	int doors_seen = 0;
	int idx = -1;
	while (doors_seen < door_chosen) {
		++idx;
		if (r->valid_doors[idx]) // put here so it leaves loop if room is found
			++doors_seen;
	}

	return idx;
}

/* Connect two sectors within the same chunk by choosing a random door on each
 * sector's room and then digging a hall between them.
 *
 * Params:
 *  cnk1, cnk2: coords of each chunk
 *	s1, s2: coords of sector within their respective chunk
 * */
void join_sectors(Coord cnk1, Coord s1, Coord cnk2, Coord s2) {
	Sector *s = scn_sector(cnk1, s1);
	Coord cur = scn_door_coord(cnk1, s1, random_door(&s->r));

	s = scn_sector(cnk2, s2);
	Coord tar = scn_door_coord(cnk2, s2, random_door(&s->r));

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
		int r = random()%2 ? -1 : 1;
		int dir[2] = {
			diff[0] == mvd[0] ? r : (diff[0] - mvd[0]) / abs(diff[0] - mvd[0]),
			diff[1] == mvd[1] ? r : (diff[1] - mvd[1]) / abs(diff[1] - mvd[1])
		};

		// next step along each axis
		Coord next[2] = {
			coord_add(cur, coord(dir[0], 0)),
			coord_add(cur, coord(0, dir[1]))
		};
 
		char *tile[2] = { tile_at(next[0]), tile_at(next[1]) };

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
				tile[prio] = tile_at(next[prio]);

				// prepare for escape condition (target reached or other axis is free)
				tmp[0] = 0; tmp[1] = 0;
				tmp[!prio] = dir[!prio];
			} while (!(mvd[prio] == diff[prio] && mvd[!prio] == diff[!prio])
			          && !tile_clear(tile_at(coord_add(cur, coord(tmp[0], tmp[1])))));
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
void gen_halls(int cy, int cx) {
	// modifiable copy
	int tmp_cnt[3][3];
	for (int y = 0; y < 3; ++y) {
		for(int x = 0; x < 3; ++x) {
			tmp_cnt[y][x] = scn.chunks[cy][cx]->sector_door_counts[y][x];
			log_fmt(LOG_GEN, "tmp_cnt[%d][%d] = %d\n", y, x, tmp_cnt[y][x]);
		}
	}

	// for each sector
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			// for each door
			while (tmp_cnt[y][x] > 0) {

				int ry, rx;
				ry = rx = 0;
				if (y == 2 && x == 2) {
					rx = random()%3;
					ry = random()%3;
				}
				else {
					rx = random()%(3-x) + x;
					ry = random()%(3-y) + y;
				}

				int abort = 0;
				while (abort < 10 && ((rx == x && ry == y)
				       || (tmp_cnt[ry][rx] == 0 && !(y == 2 && x == 2)))) {
					rx = random()%(3-x) + x;
					ry = random()%(3-y) + y;
					++abort; // prevent infinite loop
				}

				join_sectors(coord(cy, cx), coord(y, x), coord(cy, cx), coord(ry, rx));
				--tmp_cnt[y][x];
				--tmp_cnt[ry][rx];
			}
			log_fmt(LOG_GEN, "tmp_cnt[%d][%d] = %d\n", y, x, tmp_cnt[y][x]);
		}
	}
}

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
void scn_load_sector(Coord cnk, Coord sec) {
	Sector *s = scn_sector(cnk, sec);

	// Top-left corner of room in tilemap
	Coord r = coord_add(chunk_offset(cnk.y, cnk.x),
	          coord_add(sector_offset(sec.y, sec.x),
	                    coord(s->r.y, s->r.x)));

	// Draw horizontal walls (top and bottom)
  // see draw_rect() for -1 explanation
	for (int x = 0; x < s->r.width; ++x) {
		scn.tm[r.y][r.x + x] = CO_WALL;                      // top
		scn.tm[r.y + s->r.height - 1][r.x + x] = CO_WALL;    // bottom
	}

	// Draw vertical walls (left and right)
	for (int y = 0; y < s->r.height; ++y) {
		scn.tm[r.y + y][r.x] = CO_WALL;                      // left
		scn.tm[r.y + y][r.x + s->r.width - 1] = CO_WALL;     // right
	}

	// Place doors on walls
	for (int k = 0; k < 4; ++k) {
		if (!s->r.valid_doors[k])
			continue;

		Coord d = coord_add(r, s->r.doors[k]);

		// Horizontal doors: 0 (up) and 2 (down)
		if (k % 2 == 0) {
			scn.tm[d.y][d.x] =
			scn.tm[d.y][d.x + 1] =
			scn.tm[d.y][d.x + 2] = CO_CLOSED_DOOR;
		}
		// Vertical doors: 1 (right) and 3 (left)
		else {
			scn.tm[d.y][d.x] =
			scn.tm[d.y + 1][d.x] = CO_CLOSED_DOOR;
		}
	}
}

/* cy and cx are indexes into the chunk references */
void scn_load_chunk(Chunk *c, int cy, int cx) {
	// store reference
	scn.chunks[cy][cx] = c;

	// Clear chunk area in tilemap before drawing the chunk in it
	for (int y = 0; y < CHUNK_HEIGHT; ++y) {
		memset(&scn.tm[cy*CHUNK_HEIGHT + y][cx*CHUNK_WIDTH], CO_EMPTY, CHUNK_WIDTH);
	}

	// Load sectors of chunk into tilemap
	for (int sy = 0; sy < 3; ++sy) {
		for (int sx = 0; sx < 3; ++sx) {
			scn_load_sector(coord(cy, cx),
			                coord(sy, sx));
		}
	}

	// Draw halls in tilemap
	gen_halls(cy, cx);
}

void scn_init() {
	// Load (0, 0) and surrounding chunks into scn because that's default for new
	// worlds.
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Chunk *c = &world.chunks[y + WORLD_HEIGHT/2][x + WORLD_WIDTH/2];
			scn_load_chunk(c, y + 1, x + 1); // offset to stay in bounds of array
		}
	}

	// clear log and then log scene after init for debugging
	log_clear(LOG_SCN);
	for (int i = 0; i < CHUNK_HEIGHT*3; ++i) {
		log_raw(LOG_SCN, scn.tm[i], CHUNK_WIDTH*3, sizeof(char));
		log_fmt(LOG_SCN, "\n");
	}
}
