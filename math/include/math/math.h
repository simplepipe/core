#ifndef __math_math_h
#define __math_math_h

static inline float __offset(const unsigned len,
        const float *restrict l, const float *restrict r)
{
        float p;
        unsigned i;

        p = 0.0f;
        for(i = 0; i < len; ++i) {
                p += r[i] - l[i];
        }

        return p;
}

static inline void __lerp(const unsigned len, float *restrict l,
        const float *restrict r, const float t)
{
        unsigned i;
        for(i = 0; i < len; ++i) {
                l[i] += (r[i] - l[i]) * t;
        }
}

static inline void __set_scalar(const unsigned n, float * restrict l,
        const float r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] = r;
        }
}


static inline void __set(const unsigned n, float * restrict l,
        const float *restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] = r[i];
        }
}

static inline void __add(const unsigned n, float * restrict l,
        const float * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] += r[i];
        }
}

static inline void __sub(const unsigned n, float * restrict l,
        const float * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] -= r[i];
        }
}

static inline void __mul(const unsigned n, float * restrict l,
        const float * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] *= r[i];
        }
}

static inline void __div(const unsigned n, float * restrict l,
        const float * restrict r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] /= r[i];
        }
}

static inline void __add_scalar(const unsigned n, float * restrict l,
        const float r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] += r;
        }
}

static inline void __sub_scalar(const unsigned n, float * restrict l,
        const float r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] -= r;
        }
}

static inline void __mul_scalar(const unsigned n, float * restrict l,
        const float r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] *= r;
        }
}

static inline void __div_scalar(const unsigned n, float * restrict l,
        const float r)
{
        int i;
        for(i = 0; i < n; ++i) {
                l[i] /= r;
        }
}

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat3.h>
#include <math/mat4.h>
#include <math/quat.h>
#include <stdlib.h>
#include <time.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(X) ((X) * M_PI / 180)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG(X) ((X) * 180 / M_PI)
#endif

#ifndef EPSILON
#define EPSILON 0.0000000016
#endif


static inline float rand_rf(float min, float max)
{
        return ((max - min) * MIN(0.999f,((float)rand()/(float)(RAND_MAX)))) + min;
}

static inline int rand_ri(int min, int max)
{
        return (int)((max - min) * MIN(0.999f,((float)rand() / (float)RAND_MAX))) + min;
}

#endif
