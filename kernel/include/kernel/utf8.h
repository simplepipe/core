#ifndef __kernel_utf8_h
#define __kernel_utf8_h

#include <kernel/def.h>

u8 utf8_width(const char *c);
u32 utf8_code(const u8 *c_ptr, u8 width);
u8 utf8_prev_width(const char *c);

#endif
