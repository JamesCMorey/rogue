#include <ncursesw/ncurses.h>

void new_box(int y, int x, int height, int width);
void draw_rect(int y, int x, int height, int width);

typedef struct Player {
	int y, x;
} Player;

typedef struct Room {
	int y, x;
	int height, width;
} Room;


static int maxy, maxx;

int main() {
	initscr(); /* Init curses */
	cbreak(); /* Disable buffering input */
	noecho(); /* Disable echo */
	keypad(stdscr, TRUE); /* Enable F1, F2, arrow keys, etc. */
	curs_set(0); /* Hide cursor */

	getmaxyx(stdscr, maxy, maxx);

	Player p = {0, 0};

#define camy(objy) ((objy) + maxy/2 - p.y)
#define camx(objx) ((objx) + maxx/2 - p.x)


	bool running = true;
	int ch;
	while (running) {
		erase();

		for (int i = 0; i < 4; ++i) {
			draw_rect(camy(10) * i, camx(10), 5, 10);
			draw_rect(camy(10 * i), camx(-20), 5, 10);
		}
		mvaddch(maxy/2, maxx/2, '@');
		mvprintw(0, maxx - 10, "max y: %d", maxy);
		mvprintw(1, maxx - 10, "max x: %d", maxx);

		refresh();

		ch = getch();
		switch(ch) {
			case 'q': running = false; break;
			case 'w': p.y -= 1; break;
			case 'a': p.x -= 1; break;
			case 's': p.y += 1; break;
			case 'd': p.x += 1; break;
			default: break;
		}
	}


	endwin(); /* Teardown curses */
	return 0;
}

void draw_rect(int y, int x, int height, int width) {
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
		return;
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
}
