#ifndef __math_vec2_h
#define __math_vec2_h

#include <math.h>

union vec2 {
        struct { float x, y; };
        struct { float t, s; };
        struct { float width, height; };
        float v[2];
};

static inline void vec2_zero(union vec2 *restrict p)
{
        __set_scalar(2, p->v, 0);
}

static inline void vec2_neg(union vec2 * restrict p1)
{
        __mul_scalar(2, p1->v, -1);
}

static inline void vec2_add(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        __add(2, p1->v, p2->v);
}

static inline void vec2_sub(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        __sub(2, p1->v, p2->v);
}

static inline void vec2_mul(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        __mul(2, p1->v, p2->v);
}

static inline void vec2_div(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        __div(2, p1->v, p2->v);
}

static inline void vec2_add_scalar(union vec2 * restrict p1, const float n)
{
        __add_scalar(2, p1->v, n);
}

static inline void vec2_sub_scalar(union vec2 * restrict p1, const float n)
{
        __sub_scalar(2, p1->v, n);
}

static inline void vec2_mul_scalar(union vec2 * restrict p1, const float n)
{
        __mul_scalar(2, p1->v, n);
}

static inline void vec2_div_scalar(union vec2 * restrict p1, const float n)
{
        __div_scalar(2, p1->v, n);
}

static inline float vec2_length(const union vec2 * restrict p1)
{
        return (float)sqrt(p1->x * p1->x + p1->y * p1->y);
}

static inline float vec2_dot(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        return p1->x * p2->x + p1->y * p2->y;
}

static inline float vec2_dst(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        union vec2 p = *p1;
        vec2_sub(&p, p2);
        return vec2_length(&p);
}

static inline void vec2_normalize(union vec2 * restrict p1)
{
        float length = vec2_length(p1);
        vec2_mul_scalar(p1, 1.0f / length);
}

static inline float vec2_angle(union vec2 * restrict p1, const union vec2 * restrict p2)
{
        float atanA = atan2(p1->y, p1->x);
        float atanB = atan2(p2->y, p2->x);

        return atanB - atanA;
}

#endif
