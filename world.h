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

/* Structures */
typedef struct Room Room;

typedef enum {
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
} Direction;

typedef struct Pivot {
	int y, x;
	Direction dir;
} Pivot;

typedef struct Door {
	Room *r;
	int y, x;
} Door;

struct Room {
	int world_x, world_y;
	int width, height;
	int onscreen;
	Door doors[4];
};

typedef struct Hall {
	Door d1, d2;
	Pivot p[10];
} Hall;

typedef struct Chunk {
	int room_num;
	Room rooms[3][3]; /* TODO: Make this dynamic / fit the screen */
	bool onscreen;
} Chunk;

/* Player within world */
typedef struct Player {
	int y, x;
	char view[CHUNK_HEIGHT*3][CHUNK_WIDTH*3];
} Player;

/* Global state of world */
typedef struct WorldData {
	int room_num, hall_num;
	Chunk chunks[101][101];
	Hall halls[1000];
	Player player;
} WorldData; 

extern WorldData world;

void world_gen();
void world_load();
void enter_world(void *context);
