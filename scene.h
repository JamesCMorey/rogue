#pragma once

#include "geometry.h"
#include "player.h"

typedef struct Chunk Chunk;

typedef struct Scene {
	char tm[CHUNK_HEIGHT*3][CHUNK_WIDTH*3]; // active tilemap
	Chunk *chunks[3][3];                    // chunk references
	Coord player_pos;                       // player (y, x) in center chunk
} Scene;

void scn_init();
void scn_load_chunk(Chunk *c, int cy, int cx);
void scn_update(PlayerAction act);
