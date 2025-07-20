#include "gamestate.h"
#include "world.h"
#include "scene.h"
#include "log.h"
#include <stdlib.h>

GameState *gs_init() {
	log_fmt(LOG_GEN, "gs_init(): Initializing gamestate.\n");

	GameState *gs = malloc(sizeof(*gs));
	world_init(&gs->dungeon);
	scn_init(gs);

	// Send player to spawn
	pl_set_abs(&gs->player, coord(0, 0));

	return gs;
}
