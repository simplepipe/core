#ifndef __opengl_device_h
#define __opengl_device_h

struct device {
        unsigned width;
        unsigned height;

        unsigned id_sampled;
        unsigned id_resolved;
};

extern struct device g_device;

#endif
