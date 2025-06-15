#include "menu.h"
#include "vfx.h"
#include "evloop.h"
#include <ncurses.h>
#include <stdlib.h>


void main_menu_render(void *context) {
	MenuData *ctx = context;
	display_title();
	char *things[] = {"Start", "Settings", "Exit"};
	for (int i = 0; i < 3; ++i) {
		if (i == ctx->selected) {
			attron(A_ITALIC | A_BOLD | A_UNDERLINE);
			draw_txtbox(things[i], 12 + i*8, maxx/2 - 26, 7, 50);
			attroff(A_ITALIC | A_BOLD | A_UNDERLINE);
		} else
			draw_txtbox(things[i], 12 + i*8, maxx/2 - 26, 7, 50);
	}
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
