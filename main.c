#include <ncursesw/ncurses.h>

void new_box(int y, int x, int height, int width);

typedef struct Player {
	int y, x;
} Player;

typedef struct Room {
	int y, x;
	int height, width;
} Room;



int main() {
	initscr(); /* Init curses */
	cbreak(); /* Disable buffering input */
	noecho(); /* Disable echo */
	keypad(stdscr, TRUE); /* Enable F1, F2, arrow keys, etc. */
	curs_set(0); /* Hide cursor */

	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);

	Player p = {0, 0};

#define camy(objy) ((objy) + maxy/2 - p.y)
#define camx(objx) ((objx) + maxx/2 - p.x)


	bool running = true;
	int ch;
	while (running) {
		erase();

		new_box(camy(10), camx(10), 5, 10);
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

void new_box(int y, int x, int height, int width) {
	move(y, x);
	hline(0, width);
	vline(0, height);
	addch(ACS_ULCORNER);

	move(y + height, x);
	hline(0, width);
	addch(ACS_LLCORNER);

	move(y, x + width);
	vline(0, height);
	addch(ACS_URCORNER);

	move(y + height, x + width);
	addch(ACS_LRCORNER);
}
