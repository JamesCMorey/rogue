#include <curses.h>
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <wctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "vfx.h"
#include "evloop.h"

int maxy, maxx;

typedef struct AsciiArt {
	char *txt;
	int height, width;
} AsciiArt;

static AsciiArt *title = NULL; /* Store ascii art of title */

// ------ Utilities ------

void render_frame(LoopFrame *frame) {
	erase();
	frame->render(frame->ctx);
	refresh();
}

// Input wrapper for complete encapsulation of ncurses API
int get_input() { return getch(); }

// ------ Init Helpers ------

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

static void color_init() {
	if (has_colors() == FALSE) {
		endwin();
		fprintf(stderr, "Your terminal does not support color.\n");
		exit(1);
	}

	start_color();

	init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
}

// ------ Display Primatives ------

void display_title() {
	for (int r = 0; r < title->height; ++r) {
		move(r, maxx/2 - (title->width/2 + 1));
		for (int c = 0; c < title->width; ++c) {
			addch(title->txt[r * title->width + c]);
		}
	}
}

int draw_rect(int y, int x, int height, int width) {
	/* Explanation of Rect extents
	 * br = bottom right tl = top left
	 *
	 * br and tl need -1 due to the following (borders included in dimensions):
	 *
	 * Imagine there was no -1 and we had a rect (2, 2) with a width 5.
	 *
	 * tl would be (2,2), which requires no alteration as we will assume the
	 * user knows the grid starts at (0,0) and that (2,2) starts 3 cells out.
	 * br would be (2, 7)
	 *
	 * So, the leftmost wall would be drawn down from (2,2), which would be the
	 * 3rd column visually. The rightmost wall would be drawn down from (2, 7),
	 * which is the 8th column visually. This sounds fine till you realize that
	 * means the 3rd and 8th columns are the _borders_ of the rect, meaning
	 * columns 4 through 7 are contained within the rect (4 columns) and the 3rd
	 * and 8th columns are used as the edges (2 columns), ultimately resulting in
	 * a rect of width 6.
	 *
	 * The same situation occurs with height as well.
	 * */
	Coord tl = coord(y, x); // coords of top left corner
	Coord br = coord(y + height - 1, x + width - 1); // bottom right corner

	/* Clamp coords to screen
	 * These are used in partial displays (ie. when left side is off screen but
	 * right side is on screen).
	 * */
	Coord draw_tl = coord((tl.y < 0 ? 0 : tl.y), tl.x < 0 ? 0 : tl.x);
	Coord draw_br = coord((br.y > maxy ? maxy : br.y), br.x > maxx ? maxx : br.x);

	/* Offscreen--draw nothing
	 * tl.x = -10 and width = 10 ->
	 *     draw_tl.x = 0 (clamp on tl.x) and draw_br.x = 0 (tl.x + width)
	 * */
	if (draw_tl.y >= draw_br.y || draw_tl.x >= draw_br.x)
		return 0;

	/* Horizontal Edges */
	if (0 <= tl.y && tl.y < maxy) {
		move(tl.y, draw_tl.x);
		hline(0, draw_br.x - draw_tl.x);
	}
	if (0 <= br.y && br.y < maxy) {
		move(br.y, draw_tl.x);
		hline(0, draw_br.x - draw_tl.x);
	}

	/* Vertical Edges */
	if (0 <= tl.x && tl.x < maxx ) {
		move(draw_tl.y, tl.x);
		vline(0, draw_br.y - draw_tl.y);
	}
	if (0 <= br.x && br.x < maxx ) {
		move(draw_tl.y, br.x);
		vline(0, draw_br.y - draw_tl.y);
	}

	/* Draw Corners */
	if (0 <= tl.y && 0 <= tl.x) { move(tl.y, tl.x); addch(ACS_ULCORNER); }
	if (0 <= tl.y && br.x < maxx) { move(tl.y, br.x); addch(ACS_URCORNER); }
	if (br.y < maxy && 0 <= tl.x) { move(br.y, tl.x); addch(ACS_LLCORNER); }
	if (br.y < maxy && br.x < maxx) { move(br.y, br.x); addch(ACS_LRCORNER); }

	return 1;
}

void draw_txtbox(char *txt, int y, int x, int height, int width) {
	mvprintw(y + height/2, x + (width/2 - strlen(txt)/2), "%s", txt);
	draw_rect(y, x, height, width);
}

void vfx_printf(int y, int x, char *str, ...) {
	va_list args;

	move(y, x);

	va_start(args, str);
	vw_printw(stdscr, str, args);
	va_end(args);
}

void vfx_emphasis(bool on) {
	if (on) {
		attron(A_ITALIC | A_BOLD | A_UNDERLINE);
		return;
	}
	attroff(A_ITALIC | A_BOLD | A_UNDERLINE);
}

// ------ Init/Teardown ------

void vfx_init() {
	setlocale(LC_ALL, "");
	initscr(); /* Init curses */
	cbreak(); /* Disable buffering input */
	noecho(); /* Disable echo */
	keypad(stdscr, TRUE); /* Enable F1, F2, arrow keys, etc. */
	curs_set(0); /* Hide cursor */
	color_init();
	res_init();

	getmaxyx(stdscr, maxy, maxx);
}

void vfx_teardown() {
	free(title->txt);
	free(title);
	endwin(); /* Teardown curses */
}
