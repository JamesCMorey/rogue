#include <ncursesw/ncurses.h>
#include "vfx.h"

typedef struct Player {
	int y, x;
} Player;

typedef struct Room {
	int y, x;
	int height, width;
} Room;

int maxy, maxx;

int main() {
	vfx_init();
	Player p = {0, 0};

	bool running = true;
	int ch;
	while (running) {
		erase();

		draw_rect(camy(-3), camx(-9), 6, 18);

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

	vfx_teardown();
	return 0;
}
