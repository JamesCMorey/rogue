#pragma once

#include "world.h"
#include "player.h"
#include "scene.h"

typedef struct GameState {
	WorldData dungeon;
	WorldData overworld;
	Player player;
	Scene scene;
} GameState;

GameState *gs_init();
