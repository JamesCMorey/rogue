#include "menu.h"
#include "vfx.h"
#include "evloop.h"
#include "world.h"
#include "log.h"
#include "gamestate.h"
#include <stdlib.h>

// ----- Utilities ------

static void main_menu_enter_world() {
	log_fmt(LOG_GEN, "main_menu_enter_world(): Leaving menu and entering world.\n");

	WorldContext *ctx = malloc(sizeof(*ctx));
	ctx->gs = gs_init();
	ctx->exit_fn = world_ctx_teardown;

	world_enter(ctx);
}

static MenuEntry main_menu_entries[] = {
		{"Start", main_menu_enter_world},
		{"Settings", NULL},
		{"Exit", NULL}
};

// ------ Eventloop Entry ------

void main_menu_render(void *context) {
	MenuData *ctx = context;

	display_title();

	for (int i = 0; i < 3; ++i) {
		if (i == ctx->selected) {
			vfx_emphasis(true);
			draw_txtbox(ctx->entries[i].title, 12 + i*8, maxx/2 - 26, 7, 50);
			vfx_emphasis(false);
		} else
			draw_txtbox(ctx->entries[i].title, 12 + i*8, maxx/2 - 26, 7, 50);
	}
}

LogicFrameAction main_menu_logic(void *context) {
	MenuData *ctx = context;
	MenuEntry *entries = ctx->entries;

	int ch = get_input();
	switch(ch) {
		case '\n':
			if(entries[ctx->selected].switch_fn != NULL)
				(void) entries[ctx->selected].switch_fn(NULL);
			if (ctx->selected == 2)
				return LFRAME_EXIT;
			break;
		case 'j':
			if (ctx->selected < 2)
				++ctx->selected;
			break;
		case 'k':
			if (ctx->selected > 0)
				--ctx->selected;
			break;
		default:
			break;
	}
	return LFRAME_NOP;
}

void main_menu_exit(void *context) {
	log_fmt(LOG_GEN, "main_menu_exit(): Leaving main menu.\n");
	if (context)
		free(context);
}

void main_menu_enter(void *context) {
	log_fmt(LOG_GEN, "main_menu_enter(): Entering main menu.\n");

	MenuData *ctx = malloc(sizeof(*ctx));
	ctx->entries = main_menu_entries;
	ctx->selected = 0;

	eventloop_enter(ctx, main_menu_render, main_menu_logic, main_menu_exit);
}
