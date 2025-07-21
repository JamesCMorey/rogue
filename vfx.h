#pragma once

#include "player.h"

typedef struct LoopFrame LoopFrame;

extern int maxy, maxx;

static inline Coord cam_coord(Player *pl, Coord obj) {
	return coord((obj.y) + maxy/2 - pl_abs(pl).y,
	             (obj.x) + maxx/2 - pl_abs(pl).x);
}

// ------ Utilities ------
void render_frame(LoopFrame *frame);
int get_input();

// ------ Display Primatives ------
void display_title();
int draw_rect(int y, int x, int height, int width);
void draw_txtbox(char *txt, int y, int x, int height, int width);
void vfx_printf(int y, int x, char *str, ...);
void vfx_emphasis(bool on);

// ------ Init/Teardown ------
void vfx_init();
void vfx_teardown();
