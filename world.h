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

typedef struct Chunk {
	Sector sectors[3][3]; /* TODO: Make this dynamic / fit the screen */
	int sector_door_counts[3][3];
	int rng[15];
	bool exists;
} Chunk;

/* Player within world */
typedef struct Player {
	int y, x;
} Player;

/* Global state of world */
typedef struct WorldData {
	Chunk chunks[WORLD_HEIGHT][WORLD_WIDTH];
	Player player;
} WorldData; 

extern WorldData world;

void world_load();
void enter_world(void *context);
