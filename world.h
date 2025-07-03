#pragma once

#include <stdbool.h>

/* Chunk vs. Sector
 * A sector is a the smallest measured zone that is used to separate space and
 * generate rooms (a sector can either have a room or not). A chunk is made up
 * of sectors and is used for swapping in and out of the 2d-array used for 
 * rendering and simulation.
 * */
#define SECTOR_WIDTH 60
#define SECTOR_HEIGHT (SECTOR_WIDTH/2)

#define CHUNK_WIDTH (SECTOR_WIDTH*3)
#define CHUNK_HEIGHT (CHUNK_WIDTH/2)

// measured in chunks
#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100

/* Structures */
typedef struct Room Room;

typedef enum {
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
} Direction;

// typedef struct Hall {
// } Hall;

typedef struct Door {
	Room *r;
	int y, x; // with respect to the room in which the door is placed
} Door;

struct Room {
	int x, y; // with respect to the sector in which the room resides
	int width, height;
	int onscreen;
	Door doors[4];
};

typedef struct Sector {
	bool hasroom;
	Room r;
} Sector;

typedef struct Chunk {
	Sector sectors[3][3]; /* TODO: Make this dynamic / fit the screen */
	bool onscreen;
} Chunk;

/* Player within world */
typedef struct Player {
	int y, x;
	char view[CHUNK_HEIGHT*3][CHUNK_WIDTH*3];
} Player;

/* Global state of world */
typedef struct WorldData {
	Chunk chunks[WORLD_HEIGHT][WORLD_WIDTH];
	Player player;
} WorldData; 

extern WorldData world;

void world_load();
void enter_world(void *context);
