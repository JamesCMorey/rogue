#pragma once

#define camy(objy) ((objy) + maxy/2 - p.y)
#define camx(objx) ((objx) + maxx/2 - p.x)

void vfx_init();
void vfx_teardown();

void draw_rect(int y, int x, int height, int width);

extern int maxy, maxx;
