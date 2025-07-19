#pragma once

#include "geometry.h"

/* Player within world */
typedef struct Player {
	Coord abs_pos;    // absolute (y, x) in the world
	Coord cnk_pos;    // coord of chunk in which the player currently resides
	bool changed_cnk; // true if last move made player cross chunk borders
} Player;

typedef enum ActionType {
	PL_MOVE,
	PL_ATTACK,
	PL_PICKUP
} ActionType;

typedef struct PlayerAction {
	ActionType type;
	Coord movement;

} PlayerAction;

void handle_movement(char c);

void pl_set_cnk(Coord pos);
void pl_set_abs(Coord pos);
Coord pl_get_cnk();
Coord pl_get_abs();
bool pl_get_changed_cnk();
