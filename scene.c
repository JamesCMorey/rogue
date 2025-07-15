#include "scene.h"
#include "geometry.h"
#include "world.h"
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
	return scn_point(cnk, sec,
	                 coord_add(coord(s->r.y, s->r.x), s->r.doors[dir]));
}

Coord scn_start_coord(Coord cnk, Coord sec, Direction dir) {
	Coord door = scn_door_coord(cnk, sec, dir);
	switch (dir) {
		case UP:
			door = coord_add(door, coord(1, 0));
			break;
		case RIGHT:
			door = coord_add(door, coord(0, 1));
			break;
		case DOWN:
			door = coord_add(door, coord(-1, 0));
			break;
		case LEFT:
			door = coord_add(door, coord(0, -1));
			break;
	}

	return door;
}

Coord scn_end_coord(Coord cnk, Coord sec, Direction dir) {
	Coord door = scn_door_coord(cnk, sec, dir);
	switch (dir) {
		case UP:
			door = coord_add(door, coord(-1, 0));
			break;
		case RIGHT:
			door = coord_add(door, coord(0, 1));
			break;
		case DOWN:
			door = coord_add(door, coord(1, 0));
			break;
		case LEFT:
			door = coord_add(door, coord(0, -1));
			break;
	}

	return door;
}

char *tile_at(Coord c) {
	return &scn.tm[c.y][c.x];
}

bool in_room(Coord c, Coord cnk, Coord sec) {
	Room *r = &scn_sector(cnk, sec)->r;
	*tile_at(scn_point(cnk, sec, coord(r->y, r->x))) = '0';
	*tile_at(scn_point(cnk, sec, coord(r->y + r->height - 1, r->x + r->width - 1))) = '0';
	return coord_inside(c,
	            scn_point(cnk, sec, coord(r->y, r->x)),
		          // see draw_rect() for -1 explanation
	            scn_point(cnk, sec, coord(r->y + r->height - 1, r->x + r->width - 1)));
}

/*
 * Params:
 *	cnk is the chunk in which the sector resides
 *	s1 indexes of first sector
 *	s2 is the same
 * */
void join_sectors(Coord cnk, Coord s1, Coord s2) {
	Coord cur = scn_start_coord(cnk, s1, LEFT);
	Coord tar = scn_end_coord(cnk, s2, LEFT);
	Coord diff = coord_sub(tar, cur);

	int cnt = 0;
	int y = 0, x = 0;
	while ((y != diff.y || x != diff.x) && cnt < 500) {
		++cnt;

		// 1. either -1 or 1 depending on whether component is pos or neg
		// 2. (-x/y) makes the direction of travel relative to the current position 
		//        and not just the source
		int x_dir = (diff.x - x) / abs(diff.x - x);
		int y_dir = (diff.y - y) / abs(diff.y - y);

		// step from cur in direction of target
		Coord next_x = coord_add(cur, coord(0, x_dir));
		Coord next_y = coord_add(cur, coord(y_dir, 0));
 
		/* Grab potential next tiles */
		char *x_tile = tile_at(next_x);
		char *y_tile = tile_at(next_y);

		/* Take a step to the right or left if possible */
		if (x != diff.x && (*x_tile == CO_EMPTY || *x_tile == CO_HALL)) {
			*x_tile = CO_HALL;
			x += x_dir;
			cur = next_x;
		}
		else if (*y_tile == CO_EMPTY || *y_tile == CO_HALL) {
			/* If it's not possible, go up or down till it is. */
			do {
				*y_tile = CO_HALL;
				y += y_dir;
				cur = next_y;

				next_y = coord_add(cur, coord(y_dir, 0));
				y_tile = tile_at(next_y);
			} while ((x != diff.x || y != diff.y) // if either still needs fulfilling
			          && *tile_at(coord_add(cur, coord(0, x_dir))) != CO_EMPTY);
		}
	}
}

void gen_halls(Chunk *c) {
	return;
}

/* Load sector (specified by cnk and sec) from chunk references into tilemap */ 
void scn_load_sector(Coord cnk, Coord sec) {
	Sector *s = scn_sector(cnk, sec);

	// coords for room in tilemap
	Coord r = coord_add(chunk_offset(cnk.y, cnk.x),
	          coord_add(sector_offset(sec.y, sec.x),
	                    coord(s->r.y, s->r.x)));

	// draw top and bottom walls of room
	for (int x = 0; x < s->r.width; ++x) {
		scn.tm[r.y][r.x + x] = CO_WALL; // top
		// see draw_rect() for -1 explanation
		scn.tm[r.y + s->r.height - 1][r.x + x] = CO_WALL; // bottom
	}

	// draw left and right walls of room
	for (int y = 0; y < s->r.height; ++y) {
		scn.tm[r.y + y][r.x] = CO_WALL; // left
		scn.tm[r.y + y][r.x + s->r.width - 1] = CO_WALL; // right
	}

	// draw doors
	for (int k = 0; k < 4; ++k) {
		// coord of door in tilemap
		Coord d = coord_add(r, s->r.doors[k]);

		// place doors on room 
		// 0, 2 -> up and down | 1, 3 -> right and left
		if (k % 2 == 0) {
			scn.tm[d.y][d.x] =
			scn.tm[d.y][d.x + 1] =
			scn.tm[d.y][d.x + 2] = CO_CLOSED_DOOR;
		} else {
			scn.tm[d.y][d.x] =
			scn.tm[d.y + 1][d.x] = CO_CLOSED_DOOR;
		}
	}
}

/* cy and cx are indexes into the chunk references */
void scn_load_chunk(Chunk *c, int cy, int cx) {
	// store reference
	scn.chunks[cy][cx] = c;

	// zero chunk in tilemap
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
	//gen_halls(c);
}

void scn_init() {
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Chunk *c = &world.chunks[y + WORLD_HEIGHT/2][x + WORLD_WIDTH/2];
			scn_load_chunk(c, y + 1, x + 1); // offset to stay in bounds of array
		}
	}
	join_sectors(coord(0, 0),
							coord(0, 0),
							coord(0, 1));
	// join_sectors(coord(0, 0),
	// 						coord(1, 0),
	// 						coord(0, 1));
}
