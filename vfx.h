#pragma once

#include "evloop.h"
#include "player.h"

extern int maxy, maxx;

// TODO: Generalize or move this
static inline Coord cam_coord(Coord obj) {
	return coord((obj.y) + maxy/2 - pl_get_abs().y,
	             (obj.x) + maxx/2 - pl_get_abs().x);
}

void vfx_init();
void vfx_teardown();

void draw_txtbox(char *txt, int y, int x, int height, int width);
int draw_rect(int y, int x, int height, int width);
void display_title();
void render_frame(LoopFrame *frame);

int get_input();

