#ifndef __physics_particle_h
#define __physics_particle_h

#include <kernel/ref.h>
#include <math/math.h>

struct particle {
	struct ref base;
	
	union vec3 position;
	union vec3 velocity;
	union vec3 acceleration;
	union vec3 force_accum;
	
	float damping;
	float inverse_mass;
};

struct particle *particle_new();
struct particle *particle_init(struct particle *p);
void particle_release(struct particle *p);

void particle_integrate(struct particle *p, float duration);

#endif
