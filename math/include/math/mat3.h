#ifndef __math_mat3_h
#define __math_mat3_h

#include <math.h>

union mat3 {
        struct { float  m00, m01, m02,
                        m10, m11, m12,
                        m20, m21, m22; };
        struct {
                union vec3 row[3];
        };
        struct {
                union vec3 row_0;
                union vec3 row_1;
                union vec3 row_2;
        };
        float m[9];
        float mm[3][3];
};

extern union mat3 mat3_identity;

static inline void mat3_set_identity(union mat3 * restrict m)
{
        __set(9, m->m, mat3_identity.m);
}

static inline void mat3_transpose(union mat3 * restrict m)
{
        const union mat3 mb = *m;

        m->m00 = mb.m[0];
        m->m01 = mb.m[3];
        m->m02 = mb.m[6];

        m->m10 = mb.m[1];
        m->m11 = mb.m[4];
        m->m12 = mb.m[7];

        m->m20 = mb.m[2];
        m->m21 = mb.m[5];
        m->m22 = mb.m[8];
}

#endif
