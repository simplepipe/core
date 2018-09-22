#ifndef __gui_layout_h
#define __gui_layout_h

#include <kernel/array.h>
#include <opengl/pass.h>

struct gui_layout {
	struct ref base;

	union vec2 size;
	union vec2 position;
};

#endif
