#pragma once

#include <linux/limits.h>
#include <stdbool.h>

#define CHUNK_WIDTH 30
#define CHUNK_HEIGHT 15

/* Structures */
typedef struct Room Room;

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
} Hall;

/* Player within world */
typedef struct Player {
	int y, x;
} Player;

/* Global state of world */
typedef struct WorldData {
	int room_num, hall_num;
	Room rooms[100][100];
	Hall halls[1000];
	Player player;
} WorldData; 

extern WorldData world;

void world_gen();
void world_load();
void enter_world(void *context);
