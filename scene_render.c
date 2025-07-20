#include "scene_render.h"
#include "scene.h"
#include "world.h"
#include "player.h"
#include "vfx.h"
#include "gamestate.h"

// ------ Door Rendering ------

void render_doors(GameState *gs, int r_world_y, int r_world_x, Room *r) {
	if (!r->onscreen)
		return;

	for (int i = 0; i < 4; ++i) {
		if (!r->valid_doors[i])
			continue;

		Coord *d = &r->doors[i];

		Coord cam = cam_coord(&gs->player, coord(r_world_y + d->y, r_world_x + d->x));
		if (i % 2 == 0) {
			/* TODO: partial displays */
			vfx_printf(cam.y, cam.x, "+++");
		} else {
			vfx_printf(cam.y, cam.x, "+");
			vfx_printf(cam.y + 1, cam.x, "+");
		}
	}
}

// ------- Chunk Rendering -------

void render_chunk(GameState *gs, int cy, int cx) {
	WorldData *world = &gs->dungeon;
	Chunk *cnk = &world->chunks[cy + WORLD_HEIGHT/2][cx + WORLD_WIDTH/2];

	for (int sy = 0; sy < 3; ++sy) {
		for (int sx = 0; sx < 3; ++sx) {
			Room *r = &cnk->sectors[sy][sx].r;
			Coord w = world_offset(coord(cy, cx), coord(sy, sx), coord(r->y, r->x));

			Coord cam = cam_coord(&gs->player, coord(w.y, w.x));
			r->onscreen = draw_rect(cam.y, cam.x,
			                        r->height, r->width);

			render_doors(gs, w.y, w.x, r);
		}
	}
}

// ------ Scene Rendering ------

void render_chunkmap(GameState *gs) { // TODO: Make this actually draw a chunkmap
	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Coord tl = world_offset(coord(y, x), coord(0, 0), coord(0, 0));
			Coord cam = cam_coord(&gs->player, tl);
			draw_rect(cam.y, cam.x, CHUNK_HEIGHT, CHUNK_WIDTH);
		}
	}
}

void world_render(void *context) {
	WorldContext *ctx = context;
	GameState *gs = ctx->gs;

	for (int y = -1; y < 2; ++y) {
		for (int x = -1; x < 2; ++x) {
			Coord pos = abs2cnk(pl_abs(&gs->player));
			render_chunk(gs, pos.y + y, pos.x + x);
		}
	}

	render_chunkmap(gs);
	draw_rect(maxy/2, maxx/2, 2, 3); // visual square; logical nightmare
	Coord p = pl_abs(&gs->player);
	vfx_printf(maxy - 1, 0, "y:%d, x:%d", p.y, p.x);
}

void scn_render() {
	// maxx/2, maxy/2
	// abs ->
}
