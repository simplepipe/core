#include <stdlib.h>
#include <physics/particle.h>

static void __particle_free(const struct ref *ref);

struct particle *particle_new()
{
	return particle_init(malloc(sizeof(struct particle)));
}

struct particle *particle_init(struct particle *p)
{
	p->base = (struct ref){__particle_free, 1};
	vec3_zero(&p->position);
	vec3_zero(&p->velocity);
	vec3_zero(&p->acceleration);
	vec3_zero(&p->force_accum);
	p->damping = 0;
	p->inverse_mass = 1;
	return p;
}

void particle_release(struct particle *p)
{

}

static void __particle_free(const struct ref *ref)
{
	struct particle *p = cast(ref, struct particle, base);
	particle_release(p);
	free(p);
}

void particle_integrate(struct particle *p, float duration)
{
	union vec3 v;

	/*
	 * position += velocity * duration
	 */
	v = p->velocity;
	vec3_mul_scalar(&v, duration);
	vec3_add(&p->position, &v);

	/*
	 * velocity += (acceleration + Force * invmass) * duration
	 * velocity *= pow(damping, duration)
	 */
	v = p->force_accum;
	vec3_mul_scalar(&v, p->inverse_mass);
	vec3_add(&v, &p->acceleration);
	vec3_mul_scalar(&v, duration);
	vec3_add(&p->velocity, &v);
	vec3_mul_scalar(&p->velocity, pow(p->damping, duration));

	vec3_zero(&p->force_accum);
}
