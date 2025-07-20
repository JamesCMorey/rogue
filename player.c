#include "player.h"
#include "geometry.h"
#include "gamestate.h"

void move_player(GameState *gs, Coord move) {
	Coord new_abs = coord_add(pl_abs(&gs->player), move);

	Coord old_cnk = pl_cnk(&gs->player);
	Coord new_cnk = abs2cnk(new_abs);
	if (!coord_eq(old_cnk, new_cnk))
		gs->player.changed_cnk = true;

	pl_set_abs(&gs->player, new_abs);
}

PlayerAction pl_action(char c) {
	PlayerAction act;

	switch(c) {
		case 'k': case 'w': act.move = coord(-1, 0); act.type = PA_MOVE; break;
		case 'h': case 'a': act.move = coord(0, -1); act.type = PA_MOVE; break;
		case 'j': case 's': act.move = coord (1, 0); act.type = PA_MOVE; break;
		case 'l': case 'd': act.move = coord (0, 1); act.type = PA_MOVE; break;

		case 'y': act.move = coord(-1, -1); act.type = PA_MOVE; break;
		case 'u': act.move = coord(-1, 1); act.type = PA_MOVE; break;
		case 'b': act.move = coord(1, -1); act.type = PA_MOVE; break;
		case 'n': act.move = coord(1, 1); act.type = PA_MOVE; break;
		default: act.type = PA_NONE; break;
	}

	return act;
}
