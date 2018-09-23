#ifndef __gui_layout_h
#define __gui_layout_h

#include <gui/point.h>

struct gui_grid {
	struct ref base;
	
	struct gui_grid *sides[4];
	struct gui_point *coord[4];
};

#endif
