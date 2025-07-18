#pragma once

typedef struct Coord Coord;

/* Player within world */
typedef struct Player {
	int y, x;
} Player;

Coord handle_movement(char c);
