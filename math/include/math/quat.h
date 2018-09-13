#ifndef __math_quat_h
#define __math_quat_h

#include <math/vec4.h>
#include <math/mat4.h>
#include <math/vec3.h>
#include <math.h>

extern union vec4 quat_identity;
static inline void quat_mul(union vec4 * restrict q1, const union vec4 * restrict q2);
static inline void quat_conjungate(union vec4 * restrict q);
static inline void quat_angle_axis(union vec4 * restrict q, const float radian, const union vec3 * restrict v);
static inline void quat_rotate(union vec4 * restrict q, const union vec3 * restrict v, const float scale);
static inline void quat_add(union vec4 * restrict q, const union vec3 * restrict v, const float scale);
static inline void quat_from_mat4(union vec4 * restrict q, const union mat4 * restrict m);
static inline void quat_lerp(union vec4 * restrict a, const union vec4 * restrict b, const float blend);

static inline void quat_set_identity(union vec4 *p)
{
        __set(4, p->v, quat_identity.v);
}

static inline void quat_conjungate(union vec4 * restrict q)
{
        __mul_scalar(3, &q->x, -1);
}

static inline void quat_angle_axis(union vec4 * restrict q, const float radian, const union vec3 * restrict v)
{
        float half = radian * 0.5f;
        float scale = sinf(half);

        __mul(3, &q->x, v->v);
        __mul_scalar(3, &q->x, scale);
        q->w = cosf(half);
}

static inline void quat_rotate(union vec4 * restrict q, const union vec3 * restrict v, const float scale)
{
        __mul(3, &q->x, v->v);
        __mul_scalar(3, &q->x, scale);
        q->w = 0;
}

static inline void quat_add(union vec4 * restrict q, const union vec3 * restrict v, const float scale)
{
        union vec4 p = *q;
        quat_rotate(&p, v, scale);
        vec4_mul_scalar(&p, 0.5f);
        vec4_add(q, &p);
}

static inline void quat_mul(union vec4 * restrict q1, const union vec4 * restrict q2)
{
        const union vec4 q = *q1;

        q1->x = q.v[3] * q2->v[0] +
                q.v[0] * q2->v[3] +
                q.v[1] * q2->v[2] -
                q.v[2] * q2->v[1];

        q1->y = q.v[3] * q2->v[1] +
                q.v[1] * q2->v[3] +
                q.v[2] * q2->v[0] -
                q.v[0] * q2->v[2];

        q1->z = q.v[3] * q2->v[2] +
                q.v[2] * q2->v[3] +
                q.v[0] * q2->v[1] -
                q.v[1] * q2->v[0];

        q1->w = q.v[3] * q2->v[3] -
                q.v[0] * q2->v[0] -
                q.v[1] * q2->v[1] -
                q.v[2] * q2->v[2];
}

static inline void quat_from_mat4(union vec4 * restrict q, const union mat4 * restrict m)
{
        float tr =  m->m[0] + m->m[5] + m->m[10];
        if(tr > 0)
        {
                float s = sqrt(tr + 1.0) * 2;
                q->v[3] = 0.25 * s;
                q->v[0] = (m->m[9] - m->m[6]) / s;
                q->v[1] = (m->m[2] - m->m[8]) / s;
                q->v[2] = (m->m[4] - m->m[1]) / s;
        }
        else if(m->m[0] > m->m[5] && m->m[0] > m->m[10])
        {
                float s = sqrt(1.0 + m->m[0] - m->m[5] - m->m[10]) * 2;
                q->v[0] = 0.25 * s;
                q->v[1] = (m->m[4] + m->m[1]) / s;
                q->v[2] = (m->m[2] + m->m[8]) / s;
                q->v[3] = (m->m[9] - m->m[6]) / s;
        }
        else if(m->m[5] > m->m[10])
        {
                float s = sqrt(1.0 + m->m[5] - m->m[0] - m->m[10]) * 2;
                q->v[0] = (m->m[4] + m->m[1]) / s;
                q->v[1] = 0.25 * s;
                q->v[2] = (m->m[9] + m->m[6]) / s;
                q->v[3] = (m->m[2] - m->m[8]) / s;
        }
        else
        {
                float s = sqrt(1.0 + m->m[10] - m->m[0] - m->m[5]) * 2;
                q->v[0] = (m->m[2] + m->m[8]) / s;
                q->v[1] = (m->m[9] + m->m[6]) / s;
                q->v[2] = 0.25 * s;
                q->v[3] = (m->m[4] - m->m[1]) / s;
        }
}

static inline void quat_lerp(union vec4 * restrict a, const union vec4 * restrict b, const float blend)
{
	float dot = a->w * b->w + a->x * b->x + a->y * b->y + a->z * b->z;
	float blendI = 1.0f - blend;
        if (dot < 0) {
                a->w = blendI * a->w + blend * -b->w;
                a->x = blendI * a->x + blend * -b->x;
                a->y = blendI * a->y + blend * -b->y;
                a->z = blendI * a->z + blend * -b->z;
        } else {
                a->w = blendI * a->w + blend * b->w;
                a->x = blendI * a->x + blend * b->x;
                a->y = blendI * a->y + blend * b->y;
                a->z = blendI * a->z + blend * b->z;
        }
        vec4_normalize(a);
}

#endif
