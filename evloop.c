#include "evloop.h"
#include "vfx.h"
#include <stddef.h> // strictly here for NULL

struct evloop evloop;

// ------ Stack Management ------

void eventloop_enter(void *context,
                     RenderFrameFn render,
                     LogicFrameFn logic,
                     PostFrameFn exit)
{
	if (evloop.stack_ptr == NULL) {
		evloop.stack_ptr = evloop.stack;
	} else {
		++evloop.stack_ptr;
	}

	evloop.stack_ptr->ctx = context;
	evloop.stack_ptr->logic = logic;
	evloop.stack_ptr->render = render;
	evloop.stack_ptr->exit = exit;
}

void eventloop_pop() {
	if (evloop.stack_ptr == evloop.stack) { evloop.stack_ptr = NULL; }
	else { --evloop.stack_ptr; }
}

// ----- Main Eventloop ------

void eventloop_run() {
	LoopFrame *frame = evloop.stack_ptr;

	while (frame != NULL) {
		render_frame(frame);

		if (frame->logic(frame->ctx) == LFRAME_EXIT) {
			frame->exit(frame->ctx);
			eventloop_pop();
		}

		frame = evloop.stack_ptr;
	}
}
