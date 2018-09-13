#ifndef __opengl_image_h
#define __opengl_image_h

#include <kernel/ref.h>

enum {
        IMAGE_RGBA,
        IMAGE_RGB,
        IMAGE_COMPRESS_PVRTC
};

struct image {
        struct ref base;

        signed type;
        unsigned width;
        unsigned height;
        void *ptr;
};

struct image *image_new();
struct image *image_init(struct image *p);
void image_release(struct image *p);

void image_load(struct image *p, const char *file);

#endif
