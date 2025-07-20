#pragma once

#include "geometry.h"
#include "scene.h"
#include "player.h"
#include <stdbool.h>

typedef struct GameState GameState;

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
	Player player;
} WorldData;

typedef struct WorldContext {
	GameState *gs;
	void (*exit_fn)(struct WorldContext *context);
} WorldContext;

void world_init(WorldData *world);
void world_load();
void world_enter(void *context);

void world_ctx_teardown(WorldContext *context);
Chunk *world_cnk(WorldData *world, Coord cnk);
int chunk_random(Chunk *cnk);
void chunk_reset_random(Chunk *cnk);
void chunk_auto_init(GameState *gs);
