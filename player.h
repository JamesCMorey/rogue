#pragma once

#include "geometry.h"
#include "scene.h"

typedef struct Scene Scene;

/* Player within world */
typedef struct Player {
	Coord abs_pos;    // absolute (y, x) in the world
	bool changed_cnk; // true if last move made player cross chunk borders
} Player;

typedef enum ActionType {
	PA_MOVE,
	PA_ATTACK,
	PA_PICKUP,
	PA_NONE
} ActionType;

typedef struct PlayerAction {
	ActionType type;
	Coord move;
} PlayerAction;

PlayerAction pl_action(char c);
void move_player(GameState *gs, Coord move);

static inline void pl_set_abs(Player *pl, Coord pos) { pl->abs_pos = pos; }

static inline Coord pl_cnk(Player *pl) { return abs2cnk(pl->abs_pos); }
static inline Coord pl_abs(Player *pl) { return pl->abs_pos; }
static inline bool pl_changed_cnk(Player *pl) { return pl->changed_cnk; }
