#include "menu.h"
#include "vfx.h"
#include "evloop.h"
#include <stdlib.h>


void main_menu_render(void *context) {
	MenuData *ctx = context;
	display_title();
	draw_rect(12, maxx/2 - 26, 7, 50);
}

LogicFrameAction main_menu_logic(void *context) {
	MenuData *ctx = context;

	int ch = get_input();
	switch(ch) {
		case 'q': return LFRAME_EXIT; break;
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
	free(context);
}

void main_menu_create() {
	MenuData *ctx = malloc(sizeof(*ctx));
	ctx->selected = 0;
	
	eventloop_enter(ctx, main_menu_render, main_menu_logic, main_menu_exit);
}
