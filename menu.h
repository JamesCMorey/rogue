#pragma once

typedef void (*FrameSwitchFn)(void *ctx);

typedef struct MenuEntry {
	char *title;
	FrameSwitchFn switch_fn;
} MenuEntry;

typedef struct MenuData {
	int selected;
	FrameSwitchFn menu_fns[3];
	MenuEntry *entries;
} MenuData;

void main_menu_enter(void *context);
