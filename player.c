#include "player.h"
#include "geometry.h"

static Player player;

void pl_changed_cnk(Coord old) {
	old = abs2cnk(old);
	Coord new = abs2cnk(pl_get_abs());

	if (old.y == new.y && old.x == new.x) {
		player.changed_cnk = false;
		return;
	}

	player.changed_cnk = true;
}

void move_player(int y, int x) {
	int up, down, right, left;
	up = down = right = left = 1;
	// Room *r;
	// for (int i = 0; i < world.room_num; ++i) {
	// 	r = &world.chunks[50][50];
	// 	/* Check if on screen and in room */
	// 	if (r->onscreen
	// 			&& (r->world_x < p->x && p->x < r->width + r->world_x)
	// 			&& (r->world_y < p->y && p->y < r->height + r->world_y)) {
	// 		/* Check if next to wall */
	// 		if (p->y + 1 == r->world_y + r->height - 1) down = 0;
	// 		if (p->y - 1 == r->world_y) up = 0;
	// 		if (p->x - 1 == r->world_x) left = 0;
	// 		if (p->x + 1 == r->world_x + r->width - 2) right = 0;
	// 	}
	// }
	//
	// /* Move if allowed */
	// if ((y > 0 && down) || (y < 0 && up)) p->y += y;
	// if ((x > 0 && right) || (x < 0 && left)) p->x += x;
	// p->y += y;
	// p->x += x;

	Coord old = pl_get_abs(); //world.player_coord;
	pl_set_abs(coord_add(player.abs_pos, coord(y, x)));
	pl_changed_cnk(old);
}

void handle_movement(char c) {
	switch(c) {
		case 'k': case 'w': move_player(-1, 0); break;
		case 'h': case 'a': move_player(0, -1); break;
		case 'j': case 's': move_player(1, 0); break;
		case 'l': case 'd': move_player(0, 1); break;

		case 'y': move_player(-1, -1); break;
		case 'u': move_player(-1, 1); break;
		case 'b': move_player(1, -1); break;
		case 'n': move_player(1, 1); break;
		default: break;
	}
}

void pl_set_cnk(Coord cnk) {
	player.abs_pos = world_offset(cnk, coord(0, 0), coord(0, 0));
	player.cnk_pos = cnk;
}
void pl_set_abs(Coord pos) {
	player.abs_pos = pos;
	player.cnk_pos = abs2cnk(pos);
}

Coord pl_get_cnk() { return player.cnk_pos; }
Coord pl_get_abs() { return player.abs_pos; }
bool pl_get_changed_cnk() { return player.changed_cnk; }
