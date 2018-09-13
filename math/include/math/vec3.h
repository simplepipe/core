#ifndef __math_vec3_h
#define __math_vec3_h

#include <math.h>

union vec3 {
        struct {
                union {
                        struct {
                                float x;
                                float y;
                        };
                        union vec2 xy;
                };
                float z;
        };
        struct { float r, g, b; };
        struct { float width, height, depth; };
        float v[3];
};

static inline void vec3_zero(union vec3 *restrict p)
{
        __set_scalar(3, p->v, 0);
}

static inline void vec3_neg(union vec3 * restrict p1)
{
        __mul_scalar(3, p1->v, -1);
}

static inline void vec3_add(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        __add(3, p1->v, p2->v);
}

static inline void vec3_sub(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        __sub(3, p1->v, p2->v);
}

static inline void vec3_mul(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        __mul(3, p1->v, p2->v);
}

static inline void vec3_div(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        __div(3, p1->v, p2->v);
}

static inline void vec3_add_scalar(union vec3 * restrict p1, const float n)
{
        __add_scalar(3, p1->v, n);
}

static inline void vec3_sub_scalar(union vec3 * restrict p1, const float n)
{
        __sub_scalar(3, p1->v, n);
}

static inline void vec3_mul_scalar(union vec3 * restrict p1, const float n)
{
        __mul_scalar(3, p1->v, n);
}

static inline void vec3_div_scalar(union vec3 * restrict p1, const float n)
{
        __div_scalar(3, p1->v, n);
}

static inline float vec3_length(const union vec3 * restrict p1)
{
        return (float)sqrt(p1->x * p1->x + p1->y * p1->y + p1->z * p1->z);
}

static inline float vec3_dot(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        return p1->x * p2->x + p1->y * p2->y + p1->z * p2->z;
}

static inline float vec3_angle(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        return acos(vec3_dot(p1, p2) / (vec3_length(p1) * vec3_length(p2)));
}

static inline float vec3_dst(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        union vec3 p = *p2;
        vec3_sub(&p, p1);
        return vec3_length(&p);
}

static inline void vec3_cross(union vec3 * restrict p1, const union vec3 * restrict p2)
{
        const union vec3 p = *p1;
        p1->x = p.y * p2->z - p.z * p2->y;
        p1->y = p.z * p2->x - p.x * p2->z;
        p1->z = p.x * p2->y - p.y * p2->x;
}

static inline void vec3_normalize(union vec3 * restrict p1)
{
        float length = vec3_length(p1);
        vec3_mul_scalar(p1, 1.0f / length);
}

static inline void vec3_lerp(union vec3 * restrict p1, const union vec3 * restrict p2, const float progress)
{
        p1->x = p2->x * progress + (1 - progress) * p1->x;
        p1->y = p2->y * progress + (1 - progress) * p1->y;
        p1->z = p2->z * progress + (1 - progress) * p1->z;
}

#endif
