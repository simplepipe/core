#ifndef __math_vec4_h
#define __math_vec4_h

#include <math.h>

union vec4 {
        struct {
                union {
                        struct {
                                float x, y, z;
                        };
                        union vec3 xyz;
                };
                float w;
        };
        struct {
                union vec2 xy;
                union vec2 zw;
        };
        struct {
                union {
                        struct {
                                float r, g, b;
                        };
                        union vec3 rgb;
                };
                float a;
        };
        struct {
                float top, right, bottom, left;
        };
        float v[4];
};

static inline void vec4_zero(union vec4 *restrict p)
{
        __set_scalar(4, p->v, 0);
}

static inline void vec4_neg(union vec4 * restrict p1)
{
        __mul_scalar(4, p1->v, -1);
}

static inline void vec4_add(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        __add(4, p1->v, p2->v);
}

static inline void vec4_sub(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        __sub(4, p1->v, p2->v);
}

static inline void vec4_mul(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        __mul(4, p1->v, p2->v);
}

static inline void vec4_div(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        __div(4, p1->v, p2->v);
}

static inline void vec4_add_scalar(union vec4 * restrict p1, const float n)
{
        __add_scalar(4, p1->v, n);
}

static inline void vec4_sub_scalar(union vec4 * restrict p1, const float n)
{
        __sub_scalar(4, p1->v, n);
}

static inline void vec4_mul_scalar(union vec4 * restrict p1, const float n)
{
        __mul_scalar(4, p1->v, n);
}

static inline void vec4_div_scalar(union vec4 * restrict p1, const float n)
{
        __div_scalar(4, p1->v, n);
}

static inline float vec4_length(const union vec4 * restrict p1)
{
        return (float)sqrt(p1->x * p1->x + p1->y * p1->y +
                        p1->z * p1->z + p1->w * p1->w);
}

static inline float vec4_dot(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        return p1->x * p2->x + p1->y * p2->y + p1->z * p2->z + p1->w * p2->w;
}

static inline float vec4_dst(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        union vec4 p = *p2;
        vec4_sub(&p, p1);
        return vec4_length(&p);
}

static inline void vec4_cross(union vec4 * restrict p1, const union vec4 * restrict p2)
{
        const union vec4 p = *p1;
        p1->x = p.y * p2->z - p.z * p2->y;
        p1->y = p.z * p2->x - p.x * p2->z;
        p1->z = p.x * p2->y - p.y * p2->x;
        p1->w = 0.0f;
}

static inline void vec4_normalize(union vec4 * restrict p1)
{
        float length = vec4_length(p1);
        vec4_mul_scalar(p1, 1.0f / length);
}

static inline void vec4_lerp(union vec4 * restrict p1, const union vec4 * restrict p2, const float progress)
{
        p1->x = p2->x * progress + (1 - progress) * p1->x;
        p1->y = p2->y * progress + (1 - progress) * p1->y;
        p1->z = p2->z * progress + (1 - progress) * p1->z;
        p1->w = p2->w * progress + (1 - progress) * p1->w;
}

#endif
