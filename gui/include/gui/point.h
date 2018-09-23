#ifndef __gui_point_h
#define __gui_point_h

#include <kernel/ref.h>
#include <math/math.h>

struct gui_point {
	struct ref base;
	
	union vec2 value;	
};

struct gui_point *gui_point_new();
struct gui_point *gui_point_init(struct gui_point *p);
void gui_point_release(struct gui_point *p);

#endif
