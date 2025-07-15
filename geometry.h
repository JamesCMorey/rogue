#pragma once

#include <stdbool.h>

/* Chunk vs. Sector
 * A sector is a the smallest measured zone that is used to separate space and
 * generate rooms (a sector can either have a room or not). A chunk is made up
 * of sectors and is used for swapping in and out of the 2d-array used for 
 * rendering and simulation.
 * */

// measured in chars
#define SECTOR_WIDTH 60
#define SECTOR_HEIGHT (SECTOR_WIDTH/2)

// measured in sectors
#define CHUNK_WIDTH (SECTOR_WIDTH*3)
#define CHUNK_HEIGHT (CHUNK_WIDTH/2)

// measured in chunks
#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100

typedef struct {
	int y, x;
} Coord;

typedef enum {
	UP=0,
	RIGHT,
	DOWN,
	LEFT
} Direction;

typedef struct Room Room;

static inline Coord coord(int y, int x) {
	return (Coord){y, x};
}

static inline Coord coord_add(Coord a, Coord b) {
	return coord(a.y + b.y, a.x + b.x);
}

static inline Coord coord_sub(Coord a, Coord b) {
	return coord(a.y - b.y, a.x - b.x);
}

/* Assume that tl is the top left (max y) and br is bottom right (max x) */
static inline bool coord_inside(Coord a, Coord tl, Coord br) {
	return (br.y <= a.y && a.y <= tl.y) && (tl.x <= a.x && a.x <= br.x);
}

static inline Coord chunk_offset(int cy, int cx) {
	return coord(cy*CHUNK_HEIGHT, cx*CHUNK_WIDTH);
}

static inline Coord sector_offset(int sy, int sx) {
	return coord(sy*SECTOR_HEIGHT, sx*SECTOR_WIDTH);
}

static inline Coord world_offset(int cy, int cx, int sy, int sx, int y, int x) {
	return coord_add(chunk_offset(cy, cx),
	       coord_add(sector_offset(sy, sx),
	       coord_add(coord(y, x),
	                 coord(-(CHUNK_HEIGHT/2), -(CHUNK_WIDTH/2))))); // I think this may be getting used in an entirely visual manner
}

// coords of arbitrary y/x within world using y/x of chunk, sector, and offset
// #define world_y(cy, sy, y) ((cy*CHUNK_HEIGHT) + (sy*SECTOR_HEIGHT) + (y) - (CHUNK_HEIGHT/2))
// #define world_x(cx, sx, x) ((cx*CHUNK_WIDTH) + (sx*SECTOR_WIDTH) + (x) - (CHUNK_WIDTH/2))
