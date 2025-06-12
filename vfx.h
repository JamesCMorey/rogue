#pragma once

#define camy(objy) ((objy) + maxy/2 - world.player.y)
#define camx(objx) ((objx) + maxx/2 - world.player.x)

void vfx_init();
void vfx_teardown();

void render_world();

void draw_rect(int y, int x, int height, int width);

extern int maxy, maxx;
