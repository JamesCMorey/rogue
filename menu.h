#pragma once

typedef void (*FrameSwitchFn)(void *ctx);

typedef struct MenuData {
	int selected;
	FrameSwitchFn menu_fns[3];
} MenuData;

void main_menu_create(void *context);
