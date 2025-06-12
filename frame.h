#pragma once

typedef struct LoopFrame {
	void *cxt;
	void (*render_frame)(void *ctx);
	void (*logic_frame)(void *ctx);
} LoopFrame;
