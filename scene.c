#include "scene.h"
#include "geometry.h"
#include "world.h"
#include <string.h>

Scene scn;

// Using index offsets
Sector *scn_sector(int cy, int cx, int sy, int sx) {
	return &scn.chunks[cy][cx]->sectors[sy][sx];
}

Coord scn_door_coord(int cy, int cx, int sy, int sx, int dir) {
	Sector *s = scn_sector(cy, cx, sy, sx);

	return coord_add(chunk_offset(cy, cx),  // chunk offset
	       coord_add(sector_offset(sy, sx), // sector offset
	       coord_add(coord(s->r.y, s->r.x), // room offset
	                 s->r.doors[dir])));    // door offset
}

/*
 * Params:
 *	cnk is the chunk in which the sector resides
 *	s1 indexes of first sector
 *	s2 is the same
 * */
void join_sectors(Coord cnk, Coord s1, Coord s2) {
	Coord d1 = scn_door_coord(cnk.y, cnk.x, s1.y, s1.x, RIGHT);
	scn.tm[d1.y][d1.x + 1] = '#';
}

void gen_halls(Chunk *c) {
	// for (int y = 0; y < 3; ++y) {
	// 	for (int x = 0; x < 3; ++x) {
			join_sectors(coord(0, 0),
			             coord(0, 0),
			             coord(0, 1));
	// 	}
	// }
}

/* Load sector (specified by cnk and sec) from chunk references into tilemap */ 
void scn_load_sector(Coord cnk, Coord sec) {
	Sector *s = scn_sector(cnk.y, cnk.x, sec.y, sec.x);

	// coords for room in tilemap
	Coord r = coord_add(chunk_offset(cnk.y, cnk.x),
	          coord_add(sector_offset(sec.y, sec.x),
	                    coord(s->r.y, s->r.x)));

	// loops are <= bc they were not drawing bottom right corner

	// draw top and bottom walls of room
	for (int x = 0; x <= s->r.width; ++x) {
		scn.tm[r.y][r.x + x] =
		scn.tm[r.y + s->r.height][r.x + x] = CO_WALL;
	}

	// draw left and right walls of room
	for (int y = 0; y <= s->r.height; ++y) {
		scn.tm[r.y + y][r.x] =
		scn.tm[r.y + y][r.x + s->r.width] = CO_WALL;
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
	gen_halls(c);
}

void scn_init() {
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Chunk *c = &world.chunks[y + WORLD_HEIGHT/2][x + WORLD_WIDTH/2];
			scn_load_chunk(c, y + 1, x + 1); // offset to stay in bounds of array
		}
	}
}
