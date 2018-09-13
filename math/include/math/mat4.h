#ifndef __math_mat4_h
#define __math_mat4_h

union mat4 {
        struct { float  m00, m01, m02, m03,
                        m10, m11, m12, m13,
                        m20, m21, m22, m23,
                        m30, m31, m32, m33; };
        struct {
                union vec4 row[4];
        };
        struct {
                union vec4 row_0;
                union vec4 row_1;
                union vec4 row_2;
                union vec4 row_3;
        };
        float m[16];
        float mm[4][4];
};

#include <math/vec3.h>
#include <math/vec4.h>
#include <math/quat.h>
#include <math.h>

extern union mat4 mat4_identity;

static inline void mat4_inverse(union mat4 * restrict m);

static inline void mat4_transpose(union mat4 * m)
{
        const union mat4 tmp = *m;

        m->m[0] = tmp.m[0];
        m->m[1] = tmp.m[4];
        m->m[2] = tmp.m[8];
        m->m[3] = tmp.m[12];

        m->m[4] = tmp.m[1];
        m->m[5] = tmp.m[5];
        m->m[6] = tmp.m[9];
        m->m[7] = tmp.m[13];

        m->m[8] = tmp.m[2];
        m->m[9] = tmp.m[6];
        m->m[10] = tmp.m[10];
        m->m[11] = tmp.m[14];

        m->m[12] = tmp.m[3];
        m->m[13] = tmp.m[7];
        m->m[14] = tmp.m[11];
        m->m[15] = tmp.m[15];
}

static inline void mat4_mul(union mat4 * restrict m1, const union mat4 * restrict m2)
{
        const union mat4 m = *m1;

        m1->m[0]  = m.m[0] * m2->m[0]  + m.m[4] * m2->m[1]  + m.m[8] * m2->m[2]   + m.m[12] * m2->m[3];
        m1->m[4]  = m.m[0] * m2->m[4]  + m.m[4] * m2->m[5]  + m.m[8] * m2->m[6]   + m.m[12] * m2->m[7];
        m1->m[8]  = m.m[0] * m2->m[8]  + m.m[4] * m2->m[9]  + m.m[8] * m2->m[10]  + m.m[12] * m2->m[11];
        m1->m[12] = m.m[0] * m2->m[12] + m.m[4] * m2->m[13] + m.m[8] * m2->m[14]  + m.m[12] * m2->m[15];

        m1->m[1]  = m.m[1] * m2->m[0]  + m.m[5] * m2->m[1]  + m.m[9] * m2->m[2]   + m.m[13] * m2->m[3];
        m1->m[5]  = m.m[1] * m2->m[4]  + m.m[5] * m2->m[5]  + m.m[9] * m2->m[6]   + m.m[13] * m2->m[7];
        m1->m[9]  = m.m[1] * m2->m[8]  + m.m[5] * m2->m[9]  + m.m[9] * m2->m[10]  + m.m[13] * m2->m[11];
        m1->m[13] = m.m[1] * m2->m[12] + m.m[5] * m2->m[13] + m.m[9] * m2->m[14]  + m.m[13] * m2->m[15];

        m1->m[2]  = m.m[2] * m2->m[0]  + m.m[6] * m2->m[1]  + m.m[10] * m2->m[2]  + m.m[14] * m2->m[3];
        m1->m[6]  = m.m[2] * m2->m[4]  + m.m[6] * m2->m[5]  + m.m[10] * m2->m[6]  + m.m[14] * m2->m[7];
        m1->m[10] = m.m[2] * m2->m[8]  + m.m[6] * m2->m[9]  + m.m[10] * m2->m[10] + m.m[14] * m2->m[11];
        m1->m[14] = m.m[2] * m2->m[12] + m.m[6] * m2->m[13] + m.m[10] * m2->m[14] + m.m[14] * m2->m[15];

        m1->m[3]  = m.m[3] * m2->m[0]  + m.m[7] * m2->m[1]  + m.m[11] * m2->m[2]  + m.m[15] * m2->m[3];
        m1->m[7]  = m.m[3] * m2->m[4]  + m.m[7] * m2->m[5]  + m.m[11] * m2->m[6]  + m.m[15] * m2->m[7];
        m1->m[11] = m.m[3] * m2->m[8]  + m.m[7] * m2->m[9]  + m.m[11] * m2->m[10] + m.m[15] * m2->m[11];
        m1->m[15] = m.m[3] * m2->m[12] + m.m[7] * m2->m[13] + m.m[11] * m2->m[14] + m.m[15] * m2->m[15];
}

static inline void mat4_set_identity(union mat4 * restrict m)
{
        __set(16, m->m, mat4_identity.m);
}

static inline void mat4_new_translation(union mat4 * restrict m, const union vec3 * restrict v)
{
        __set(16, m->m, mat4_identity.m);
        __set(3, &m->m[12], v->v);
}

static inline void mat4_new_scale(union mat4 * restrict m, const union vec3 * restrict v)
{
        __set(16, m->m, mat4_identity.m);
        m->m[0]  = v->x;
        m->m[5]  = v->y;
        m->m[10] = v->z;
}

static inline void __mat4_new_rotation(union mat4 * restrict m, const float radians, const union vec3 * restrict v)
{
        const float cos = cosf(radians);
        const float cosp = 1.0f - cos;
        const float sin = sinf(radians);

        m->m00 = cos + cosp * v->v[0] * v->v[0];
        m->m01 = cosp * v->v[0] * v->v[1] + v->v[2] * sin;
        m->m02 = cosp * v->v[0] * v->v[2] - v->v[1] * sin;
        m->m03 = 0.0f;

        m->m10 = cosp * v->v[0] * v->v[1] - v->v[2] * sin;
        m->m11 = cos + cosp * v->v[1] * v->v[1];
        m->m12 = cosp * v->v[1] * v->v[2] + v->v[0] * sin;
        m->m13 = 0.0f;

        m->m20 = cosp * v->v[0] * v->v[2] + v->v[1] * sin;
        m->m21 = cosp * v->v[1] * v->v[2] - v->v[0] * sin;
        m->m22 = cos + cosp * v->v[2] * v->v[2];
        m->m23 = 0.0f;

        m->m30 = 0.0f;
        m->m31 = 0.0f;
        m->m32 = 0.0f;
        m->m33 = 1.0f;
}

static inline void mat4_new_rotation(union mat4 * restrict m, const float radians, const union vec3 * restrict vc)
{
        union vec3 v = *vc;
        vec3_normalize(&v);

        __mat4_new_rotation(m, radians, &v);
}

static inline void mat4_new_x_rotation(union mat4 *restrict m, const float radians)
{
        const float cos = cosf(radians);
        const float sin = sinf(radians);

        m->m[0] = 1.0f;
        m->m[1] = 0.0f;
        m->m[2] = 0.0f;
        m->m[3] = 0.0f;

        m->m[4] = 0.0f;
        m->m[5] = cos;
        m->m[6] = sin;
        m->m[7] = 0.0f;

        m->m[8] = 0.0f;
        m->m[9] = -sin;
        m->m[10] = cos;
        m->m[11] = 0.0f;

        m->m[12] = 0.0f;
        m->m[13] = 0.0f;
        m->m[14] = 0.0f;
        m->m[15] = 1.0;
}

static inline void mat4_new_y_rotation(union mat4 *restrict m, const float radians)
{
        const float cos = cosf(radians);
        const float sin = sinf(radians);

        m->m[0] = cos;
        m->m[1] = 0.0f;
        m->m[2] = -sin;
        m->m[3] = 0.0f;

        m->m[4] = 0.0f;
        m->m[5] = 1.0f;
        m->m[6] = 0.0f;
        m->m[7] = 0.0f;

        m->m[8] = sin;
        m->m[9] = 0.0f;
        m->m[10] = cos;
        m->m[11] = 0.0f;

        m->m[12] = 0.0f;
        m->m[13] = 0.0f;
        m->m[14] = 0.0f;
        m->m[15] = 1.0f;
}

static inline void mat4_new_z_rotation(union mat4 *restrict m, const float radians)
{
        const float cos = cosf(radians);
        const float sin = sinf(radians);

        m->m[0] = cos;
        m->m[1] = sin;
        m->m[2] = 0.0f;
        m->m[3] = 0.0f;

        m->m[4] = -sin;
        m->m[5] = cos;
        m->m[6] = 0.0f;
        m->m[7] = 0.0f;

        m->m[8] = 0.0f;
        m->m[9] = 0.0f;
        m->m[10] = 1.0f;
        m->m[11] = 0.0f;

        m->m[12] = 0.0f;
        m->m[13] = 0.0f;
        m->m[14] = 0.0f;
        m->m[15] = 1.0f;
}

static inline void mat4_new_perspective(union mat4 *restrict m, const float fovyRadians,
        const float aspect, const float nearZ, const float farZ)
{
        const float cotan = 1.0f / tanf(fovyRadians / 2.0f);
        m->m[0] = cotan / aspect;
        m->m[1] = 0.0f;
        m->m[2] = 0.0f;
        m->m[3] = 0.0f;

        m->m[4] = 0.0f;
        m->m[5] = cotan;
        m->m[6] = 0.0f;
        m->m[7] = 0.0f;

        m->m[8] = 0.0f;
        m->m[9] = 0.0f;
        m->m[10] = (farZ + nearZ) / (nearZ - farZ);
        m->m[11] = -1.0f;

        m->m[12] = 0.0f;
        m->m[13] = 0.0f;
        m->m[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
        m->m[15] = 0.0f;
}

static inline void mat4_new_ortho(union mat4 * restrict m,
                                        const float left, const float right,
                                        const float bottom, const float top,
                                        const float nearZ, const float farZ)
{
        const float ral = right + left;
        const float rsl = right - left;
        const float tab = top + bottom;
        const float tsb = top - bottom;
        const float fan = farZ + nearZ;
        const float fsn = farZ - nearZ;

        m->m00 = 2.0f / rsl;
        m->m01 = 0.0f;
        m->m02 = 0.0f;
        m->m03 = 0.0f;

        m->m10 = 0.0f;
        m->m11 = 2.0f / tsb;
        m->m12 = 0.0f;
        m->m13 = 0.0f;

        m->m20 = 0.0f;
        m->m21 = 0.0f;
        m->m22 = -2.0f / fsn;
        m->m23 = 0.0f;

        m->m30 = -ral / rsl;
        m->m31 = -tab / tsb;
        m->m32 = -fan / fsn;
        m->m33 = 1.0f;
}

static inline void mat4_new_look_at(union mat4 * restrict m,
        const union vec3 * restrict eye,
        const union vec3 * restrict center,
        const union vec3 * restrict up)
{
        float d;
        union vec3 n, u, v;

        n = *eye;
        vec3_sub(&n, center);
        vec3_normalize(&n);

        u = *up;
        vec3_cross(&u, &n);

        v = n;
        vec3_cross(&v, &u);

        m->m00 = u.v[0];
        m->m01 = v.v[0];
        m->m02 = n.v[0];
        m->m03 = 0.0f;

        m->m10 = u.v[1];
        m->m11 = v.v[1];
        m->m12 = n.v[1];
        m->m13 = 0.0f;

        m->m20 = u.v[2];
        m->m21 = v.v[2];
        m->m22 = n.v[2];
        m->m23 = 0.0f;

        vec3_neg(&u);
        m->m30 = vec3_dot(&u, eye);

        vec3_neg(&v);
        m->m31 = vec3_dot(&v, eye);

        vec3_neg(&n);
        m->m32 = vec3_dot(&n, eye);

        m->m33 = 1.0f;
}

static inline void mat4_translate(union mat4 * restrict m, const union vec3 * restrict p)
{
        m->m[12] += m->m[0] * p->v[0] + m->m[4] * p->v[1] + m->m[8] * p->v[2];
        m->m[13] += m->m[1] * p->v[0] + m->m[5] * p->v[1] + m->m[7] * p->v[2];
        m->m[14] += m->m[2] * p->v[0] + m->m[6] * p->v[1] + m->m[8] * p->v[2];
}

static inline void mat4_scale(union mat4 * restrict m, const union vec3 * restrict p)
{
        vec4_mul_scalar(&m->row[0], p->v[0]);
        vec4_mul_scalar(&m->row[1], p->v[1]);
        vec4_mul_scalar(&m->row[2], p->v[2]);
}

static inline void mat4_rotate(union mat4 * restrict m,
        const float radian, const union vec3 * restrict p)
{
        union mat4 r;

        mat4_new_rotation(&r, radian, p);
        mat4_mul(m, &r);
}

static inline void mat4_rotate_x(union mat4 * restrict m, const float radian)
{
        union mat4 r;

        mat4_new_x_rotation(&r, radian);
        mat4_mul(m, &r);
}

static inline void mat4_rotate_y(union mat4 * restrict m, const float radian)
{
        union mat4 r;

        mat4_new_y_rotation(&r, radian);
        mat4_mul(m, &r);
}

static inline void mat4_rotate_z(union mat4 * restrict m, const float radian)
{
        union mat4 r;

        mat4_new_z_rotation(&r, radian);
        mat4_mul(m, &r);
}

static inline void mat4_mul_vec4(const union mat4 * restrict m, union vec4 * restrict r)
{
        const union vec4 p = *r;

        r->v[0] = m->m[0] * p.v[0] + m->m[4] * p.v[1] + m->m[8] * p.v[2] + m->m[12] * p.v[3];
        r->v[1] = m->m[1] * p.v[0] + m->m[5] * p.v[1] + m->m[9] * p.v[2] + m->m[13] * p.v[3];
        r->v[2] = m->m[2] * p.v[0] + m->m[6] * p.v[1] + m->m[10] * p.v[2] + m->m[14] * p.v[3];
        r->v[3] = m->m[3] * p.v[0] + m->m[7] * p.v[1] + m->m[11] * p.v[2] + m->m[15] * p.v[3];
}

static inline void mat4_mul_vec3_translation(const union mat4 * restrict m,
        union vec3 * restrict p)
{
        union vec4 t;

        t.xyz = *p;
        t.w = 1.0f;
        mat4_mul_vec4(m, &t);

        *p = t.xyz;
}

static inline void mat4_mul_project_vec3(const union mat4 * restrict m,
        union vec3 * restrict p)
{
        union vec4 t;

        t.xyz = *p;
        t.w = 1.0f;
        mat4_mul_vec4(m, &t);
        vec3_mul_scalar(&t.xyz, 1.0f / t.w);

        *p = t.xyz;
}

static inline void mat4_invert_mul_project_vec3(const union mat4 * restrict m,
        union vec3 * restrict p)
{
        union mat4 im;
        union vec4 t;

        im = *m;
        mat4_inverse(&im);

        t.xyz = *p;
        t.w = 1.0f;
        mat4_mul_vec4(&im, &t);
        vec3_mul_scalar(&t.xyz, 1.0f / t.w);

        *p = t.xyz;
}

static inline void mat4_transform_direction(const union mat4 * restrict m,
        union vec3 * restrict p)
{
        union vec3 t;

        t = *p;

        p->x = t.x * m->m[0] + t.y * m->m[1] + t.z * m->m[2];
        p->y = t.x * m->m[4] + t.y * m->m[5] + t.z * m->m[6];
        p->z = t.x * m->m[8] + t.y * m->m[9] + t.z * m->m[10];
}

static inline void mat4_transform_inverse_direction(const union mat4 * restrict m,
        union vec3 * restrict p)
{
        union vec3 t;

        t = *p;

        p->x = t.x * m->m[0] + t.y * m->m[4] + t.z * m->m[8];
        p->y = t.x * m->m[1] + t.y * m->m[5] + t.z * m->m[9];
        p->z = t.x * m->m[2] + t.y * m->m[6] + t.z * m->m[10];
}

static inline void mat4_transform_to_world_direction(
        const union mat4 * restrict m,
        union vec3 * restrict p)
{
        mat4_transform_direction(m, p);
}

static inline void mat4_transform_to_local_direction(
        const union mat4 * restrict m,
        union vec3 * restrict p)
{
        mat4_transform_inverse_direction(m, p);
}

static inline void mat4_from_quat(union mat4 *restrict m,
        const union vec4 * restrict q)
{
        union vec4 qn;

        qn = *q;
        vec4_normalize(&qn);

        const float x = qn.x;
        const float y = qn.y;
        const float z = qn.z;
        const float w = qn.w;

        const float _2x = x + x;
        const float _2y = y + y;
        const float _2z = z + z;
        const float _2w = w + w;

        m->m00 = 1.0f - _2y * y - _2z * z;
        m->m01 = _2x * y + _2w * z;
        m->m02 = _2x * z - _2w * y;
        m->m03 = 0.0f;

        m->m10 = _2x * y - _2w * z;
        m->m11 = 1.0f - _2x * x - _2z * z;
        m->m12 = _2y * z + _2w * x;
        m->m13 = 0.0f;

        m->m20 = _2x * z + _2w * y;
        m->m21 = _2y * z - _2w * x;
        m->m22 = 1.0f - _2x * x - _2y * y;
        m->m23 = 0.0f;

        m->m30 = 0.0f;
        m->m31 = 0.0f;
        m->m32 = 0.0f;
        m->m33 = 1.0f;
}

static inline float mat4_det(const union mat4 * restrict m)
{
        const float s0 = m->m00 * m->m11 - m->m10 * m->m01;
        const float s1 = m->m00 * m->m12 - m->m10 * m->m02;
        const float s2 = m->m00 * m->m13 - m->m10 * m->m03;
        const float s3 = m->m01 * m->m12 - m->m11 * m->m02;
        const float s4 = m->m01 * m->m13 - m->m11 * m->m03;
        const float s5 = m->m02 * m->m13 - m->m12 * m->m03;

        const float c5 = m->m22 * m->m33 - m->m32 * m->m23;
        const float c4 = m->m21 * m->m33 - m->m31 * m->m23;
        const float c3 = m->m21 * m->m32 - m->m31 * m->m22;
        const float c2 = m->m20 * m->m33 - m->m30 * m->m23;
        const float c1 = m->m20 * m->m32 - m->m30 * m->m22;
        const float c0 = m->m20 * m->m31 - m->m30 * m->m21;

        return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

static inline void mat4_inverse(union mat4 * restrict m)
{
        const union mat4 mb = *m;
        const float s0 = mb.m00 * mb.m11 - mb.m10 * mb.m01;
        const float s1 = mb.m00 * mb.m12 - mb.m10 * mb.m02;
        const float s2 = mb.m00 * mb.m13 - mb.m10 * mb.m03;
        const float s3 = mb.m01 * mb.m12 - mb.m11 * mb.m02;
        const float s4 = mb.m01 * mb.m13 - mb.m11 * mb.m03;
        const float s5 = mb.m02 * mb.m13 - mb.m12 * mb.m03;

        const float c5 = mb.m22 * mb.m33 - mb.m32 * mb.m23;
        const float c4 = mb.m21 * mb.m33 - mb.m31 * mb.m23;
        const float c3 = mb.m21 * mb.m32 - mb.m31 * mb.m22;
        const float c2 = mb.m20 * mb.m33 - mb.m30 * mb.m23;
        const float c1 = mb.m20 * mb.m32 - mb.m30 * mb.m22;
        const float c0 = mb.m20 * mb.m31 - mb.m30 * mb.m21;
        const float det = s0 * c5 - s1 * c4 + s2 * c3 +
                s3 * c2 - s4 * c1 + s5 * c0;

        if(det == 0.0f) return;

        __set_scalar(16, m->m, 1.0f / det);

        m->m00 *= (mb.m11 * c5 - mb.m12 * c4 + mb.m13 * c3);
        m->m01 *= (-mb.m01 * c5 + mb.m02 * c4 - mb.m03 * c3);
        m->m02 *= (mb.m31 * s5 - mb.m32 * s4 + mb.m33 * s3);
        m->m03 *= (-mb.m21 * s5 + mb.m22 * s4 - mb.m23 * s3);

        m->m10 *= (-mb.m10 * c5 + mb.m12 * c2 - mb.m13 * c1);
        m->m11 *= (mb.m00 * c5 - mb.m02 * c2 + mb.m03 * c1);
        m->m12 *= (-mb.m30 * s5 + mb.m32 * s2 - mb.m33 * s1);
        m->m13 *= (mb.m20 * s5 - mb.m22 * s2 + mb.m23 * s1);

        m->m20 *= (mb.m10 * c4 - mb.m11 * c2 + mb.m13 * c0);
        m->m21 *= (-mb.m00 * c4 + mb.m01 * c2 - mb.m03 * c0);
        m->m22 *= (mb.m30 * s4 - mb.m31 * s2 + mb.m33 * s0);
        m->m23 *= (-mb.m20 * s4 + mb.m21 * s2 - mb.m23 * s0);

        m->m30 *= (-mb.m10 * c3 + mb.m11 * c1 - mb.m12 * c0);
        m->m31 *= (mb.m00 * c3 - mb.m01 * c1 + mb.m02 * c0);
        m->m32 *= (-mb.m30 * s3 + mb.m31 * s1 - mb.m32 * s0);
        m->m33 *= (mb.m20 * s3 - mb.m21 * s1 + mb.m22 * s0);
}

static inline void mat4_lerp(union mat4 * restrict m1,
        const union mat4 * restrict m2, const float progress)
{
        union mat4 mb;
        union vec4 q, q2;
        union vec3 p, s, t;

        quat_from_mat4(&q, m1);
        quat_from_mat4(&q2, m2);
        quat_lerp(&q, &q2, progress);

        p = m1->row[3].xyz;
        vec3_lerp(&p, &m2->row[3].xyz, progress);

        s.x = vec3_length(&m1->row[0].xyz);
        s.y = vec3_length(&m1->row[1].xyz);
        s.z = vec3_length(&m1->row[2].xyz);

        t.x = vec3_length(&m2->row[0].xyz);
        t.y = vec3_length(&m2->row[1].xyz);
        t.z = vec3_length(&m2->row[2].xyz);

        vec3_lerp(&s, &t, progress);

        mat4_from_quat(m1, &q);
        mat4_transpose(m1);
        mat4_new_scale(&mb, &s);
        mat4_mul(m1, &mb);

        mb = *m1;
        mat4_new_translation(m1, &p);
        mat4_mul(m1, &mb);
}

#endif
