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

typedef struct Coord {
	int y, x;
} Coord;

typedef enum {
	UP=0,
	RIGHT,
	DOWN,
	LEFT,
	DIR_COUNT
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

/* Assume that tl is the top left (min y/x) and br is bottom right (max y/x) */
static inline bool coord_inside(Coord a, Coord tl, Coord br) {
	return (tl.y <= a.y && a.y < br.y) && (tl.x <= a.x && a.x <= br.x);
}

static inline Coord chunk_offset(int cy, int cx) {
	return coord(cy*CHUNK_HEIGHT, cx*CHUNK_WIDTH);
}

static inline Coord sector_offset(int sy, int sx) {
	return coord(sy*SECTOR_HEIGHT, sx*SECTOR_WIDTH);
}

/* Convert absolute (y, x) to cnk index (y, x). Example conversion for x using
 * CHUNK_WIDTH=180:
 * ...
 * (-450, -270] -> -2
 * (-270, -90] -> -1
 * (-90, 90) -> 0
 * [90, 270) -> 1
 * [270, 360) -> 2
 * ...
 * */
static inline Coord abs2cnk(Coord pnt) {
	return coord(
		(pnt.y > 0 ? pnt.y+(CHUNK_HEIGHT/2) : pnt.y-(CHUNK_HEIGHT/2))/CHUNK_HEIGHT,
		(pnt.x > 0 ? pnt.x+(CHUNK_WIDTH/2) : pnt.x-(CHUNK_WIDTH/2))/CHUNK_WIDTH
	);
}

// convert chunk, sector, and point indexes into an absolute (y, x)
static inline Coord world_offset(Coord cnk, Coord sec, Coord pnt) {
	return coord_add(chunk_offset(cnk.y, cnk.x),
	       coord_add(sector_offset(sec.y, sec.x),
	       coord_add(coord(pnt.y, pnt.x),
	                 coord(-(CHUNK_HEIGHT/2), -(CHUNK_WIDTH/2)))));
}
