#include "player.h"
#include "geometry.h"

Coord move_player(int y, int x) {
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
	return coord(y, x);
}

Coord handle_movement(char c) {
	switch(c) {
		case 'k': case 'w': return move_player(-1, 0); break;
		case 'h': case 'a': return move_player(0, -1); break;
		case 'j': case 's': return move_player(1, 0); break;
		case 'l': case 'd': return move_player(0, 1); break;

		case 'y': return move_player(-1, -1); break;
		case 'u': return move_player(-1, 1); break;
		case 'b': return move_player(1, -1); break;
		case 'n': return move_player(1, 1); break;
		default: return coord(0, 0); break;
	}
}
