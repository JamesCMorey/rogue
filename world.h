#pragma once

#include "geometry.h"
#include "scene.h"
#include <stdbool.h>

typedef enum {
	CO_EMPTY = ' ',
	CO_WALL = '|',
	CO_CLOSED_DOOR = '+',
	CO_OPEN_DOOR = '_',
	CO_HALL = '#'
} CharObject;

typedef struct Room {
	int x, y; // with respect to the sector in which the room resides
	int width, height;
	int onscreen;
	Coord doors[4];
	bool valid_doors[4];
	int door_num;
} Room;

typedef struct {
	bool hasroom;
	Room r;
} Sector;


#define CNK_RNG_CNT 15

typedef struct Chunk {
	Sector sectors[3][3]; /* TODO: Make this dynamic / fit the screen */
	int sector_door_counts[3][3];
	int rng[CNK_RNG_CNT];
	int rng_itr;
	bool rng_full;
	bool initialized;
} Chunk;

/* Global state of world */
typedef struct WorldData {
	Chunk chunks[WORLD_HEIGHT][WORLD_WIDTH];
	Coord player_coord;
} WorldData;

extern WorldData world;

void world_load();
void enter_world(void *context);

static inline Coord cnk2idx(Coord cnk) {
	return coord(cnk.y + WORLD_HEIGHT/2, cnk.x + WORLD_WIDTH/2);
}

Chunk *world_get_cnk(Coord cnk);
int chunk_random(Chunk *cnk);
