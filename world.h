#pragma once

#define CHUNK_WIDTH 30
#define CHUNK_HEIGHT 15

/* Structures */
typedef struct Room {
	int world_x, world_y;
	int width, height;
	int onscreen;
} Room;

typedef struct Hall {
	int r1_x, r1_y;
	int r2_x, r2_y;
} Hall;

/* Player within world */
typedef struct Player {
	int y, x;
} Player;

/* Global state of world */
typedef struct WorldData {
	int room_num, hall_num;
	Room rooms[100];
	Hall halls[1000];
	Player player;
} WorldData; 

extern WorldData world;

void world_gen();
void world_load();
void enter_world(void *context);
