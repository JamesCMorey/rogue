#include "vfx.h"
#include "world.h"
#include "evloop.h"
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>

int maxy, maxx;

typedef struct AsciiArt {
	char *txt;
	int height, width;
} AsciiArt;
static AsciiArt *title = NULL; /* Store ascii art of title */

static void color_init() {
	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Your terminal does not support color.\n");
		exit(1);
	}

	start_color();

	init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
}

void display_title() {
	for (int r = 0; r < title->height; ++r) {
		move(r, maxx/2 - (title->width/2 + 1));
		for (int c = 0; c < title->width; ++c) {
			addch(title->txt[r * title->width + c]);
		}
	}
}

static void res_init() {
	title = malloc(sizeof(*title));
	title->txt = malloc(sizeof(char) * 1024);

	FILE *fd = fopen("vfx_res", "r");

	/* Title will always be first thing in resources file */
	int itr = 0;
	wint_t ch, prev = 0;
	bool running = true, width_saved = false;
	while (running && (ch = fgetwc(fd)) != WEOF) {
		if (prev == '\n' && ch == '\n') { /* Exit on double \n */
			running = false;
		}
		else if (ch == '\n') { /* Skip \n */
			if (!width_saved)
				title->width = itr;
			width_saved = true;
		}
		else { /* Save non-\n char */
			title->txt[itr] = ch;
			++itr;
		}

		prev = ch;
	}

	title->height = itr/title->width;
	title->txt[itr] = 0;

	fclose(fd);
}

void vfx_init() {
	setlocale(LC_ALL, "");
	initscr(); /* Init curses */
	cbreak(); /* Disable buffering input */
	noecho(); /* Disable echo */
	keypad(stdscr, TRUE); /* Enable F1, F2, arrow keys, etc. */
	curs_set(0); /* Hide cursor */
	color_init();
	res_init();
	//attron(COLOR_PAIR(1));

	getmaxyx(stdscr, maxy, maxx);
}

void vfx_teardown() {
	free(title);
	endwin(); /* Teardown curses */
}

// void render_world() {
// 		erase();
//
// 		for (int r = 0; r < 3; ++r) {
// 			for (int c = 0; c < 3; ++c)
// 				draw_rect(camy(20 * r), camx(40 * c), 20, 40);
// 		}
//
// 		draw_rect(maxy/2, maxx/2, 1, 2);
// 		// mvprintw(0, maxx - 10, "max y: %d", maxy);
// 		// mvprintw(1, maxx - 10, "max x: %d", maxx);
// 		refresh();
// }

void render_frame(LoopFrame *frame) {
	erase();
	frame->render(frame->ctx);
	refresh();
}

/* TODO maybe find a more appropriate spot for this */
int get_input() {
	return getch();
}

int draw_rect(int y, int x, int height, int width) {
	/* Rect extents */
	int x0 = x;
	int y0 = y;
	int x1 = x + width;
	int y1 = y + height;

	/* Clamp coords to screen */
	int draw_x0 = x0 < 0 ? 0 : x0;
	int draw_y0 = y0 < 0 ? 0 : y0;
	int draw_x1 = x1 > maxx ? maxx : x1;
	int draw_y1 = y1 > maxy ? maxy : y1;

	/* Offscreen--draw nothing */
	if (draw_y0 >= draw_y1 || draw_x0 >= draw_x1) {
		return 0;
	}

	/* Horizontal Edges */
	if (0 <= y0 && y0 < maxy) {
		move(y0, draw_x0);
		hline(0, draw_x1 - draw_x0);
	}
	if (0 <= y1 && y1 < maxy) {
		move(y1, draw_x0);
		hline(0, draw_x1 - draw_x0);
	}

	/* Vertical Edges */
	if (0 <= x0 && x0 < maxx ) {
		move(draw_y0, x0);
		vline(0, draw_y1 - draw_y0);
	}
	if (0 <= x1 && x1 < maxx ) {
		move(draw_y0, x1);
		vline(0, draw_y1 - draw_y0);
	}

	/* Draw Corners */
	if (0 <= y0 && 0 <= x0) { move(y0, x0); addch(ACS_ULCORNER); }
	if (0 <= y0 && x1 < maxx) { move(y0, x1); addch(ACS_URCORNER); }
	if (y1 < maxy && 0 <= x0) { move(y1, x0); addch(ACS_LLCORNER); }
	if (y1 < maxy && x1 < maxx) { move(y1, x1); addch(ACS_LRCORNER); }

	return 1;
}

void draw_txtbox(char *txt, int y, int x, int height, int width) {
	mvprintw(y + height/2, x + (width/2 - strlen(txt)/2), "%s", txt);
	draw_rect(y, x, height, width);
}
