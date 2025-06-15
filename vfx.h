#pragma once

#include "evloop.h"

#define camy(objy) ((objy) + maxy/2 - world.player.y)
#define camx(objx) ((objx) + maxx/2 - world.player.x)

void vfx_init();
void vfx_teardown();

void draw_txtbox(char *txt, int y, int x, int height, int width);
int draw_rect(int y, int x, int height, int width);
void display_title();
void render_frame(LoopFrame *frame);

int get_input();

extern int maxy, maxx;
