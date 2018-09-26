#ifndef __physics_pfgen_h
#define __physics_pfgen_h

#include <physics/particle.h>

struct particle_force_generator {
        struct ref base;
        
        void(*update)(struct particle *, float duration);
};

struct particle_force_registry {
        struct ref base;

        
};

#endif
