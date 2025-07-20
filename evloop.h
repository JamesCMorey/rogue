#pragma once

typedef enum LogicFrameAction {
	LFRAME_EXIT,
	LFRAME_NOP
} LogicFrameAction;

typedef void (*RenderFrameFn)(void *context);
typedef LogicFrameAction (*LogicFrameFn)(void *context);
typedef void (*PostFrameFn)(void *context);

typedef struct LoopFrame {
	void *ctx;
	RenderFrameFn render;
	LogicFrameFn logic;
	PostFrameFn exit;
} LoopFrame;

extern struct evloop {
	LoopFrame stack[10];
	LoopFrame *stack_ptr;
} evloop;

void eventloop_run();
void eventloop_enter(void *context,
                     RenderFrameFn render,
                     LogicFrameFn logic,
                     PostFrameFn exit);
