#include <ncursesw/ncurses.h>
#include "vfx.h"
#include "world.h"
#include "frame.h"

void handle_input(bool *running) {
		char ch = getch();
		switch(ch) {
			case 'q': *running = false; break;
			case 'k':
			case 'w': world.player.y -= 1; break;
			case 'h':
			case 'a': world.player.x -= 1; break;
			case 'j':
			case 's': world.player.y += 1; break;
			case 'l':
			case 'd': world.player.x += 1; break;
			default: break;
		}
}

int main() {
	vfx_init();

	bool running = true;
	while (running) {
		render_world();
		handle_input(&running);
	}

	vfx_teardown();
	return 0;
}
