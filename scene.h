#pragma once

#include "geometry.h"

#define SCN_HEIGHT 3
#define SCN_WIDTH 3

typedef struct Chunk Chunk;
typedef struct PlayerAction PlayerAction;
typedef struct GameState GameState;

typedef struct Scene {
	char tm[CHUNK_HEIGHT*SCN_HEIGHT][CHUNK_WIDTH*SCN_WIDTH]; // active tilemap
	Chunk *chunks[SCN_HEIGHT][SCN_WIDTH];                    // chunk references
} Scene;

void scn_init(GameState *gs);
void scn_load_chunk(Scene *scn, Chunk *c, int cy, int cx);
void scn_update(GameState *gs, PlayerAction act);
