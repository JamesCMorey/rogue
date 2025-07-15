#include "vfx.h"
#include "world.h"
#include "evloop.h"
#include <locale.h>
#include <ncursesw/ncurses.h>
#include <wchar.h>
#include <wctype.h>
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
	/* Explanation of Rect extents
	 *
	 * br needs -1 because of the following (borders included in dimensions):
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

