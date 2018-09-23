#include <stdlib.h>
#include <gui/point.h>

static void __gui_point_free(const struct ref *ref);

struct gui_point *gui_point_new()
{
	return gui_point_init(malloc(sizeof(struct gui_point)));
}

struct gui_point *gui_point_init(struct gui_point *p)
{
	p->base = (struct ref){__gui_point_free, 1};
	vec2_zero(&p->value);
	return p;
}

void gui_point_release(struct gui_point *p)
{
}

static void __gui_point_free(const struct ref *ref)
{
	struct gui_point *p = cast(ref, struct gui_point, base);
	gui_point_release(p);
	free(p);
}
