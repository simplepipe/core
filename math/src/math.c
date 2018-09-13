#include <math/math.h>

union mat4 mat4_identity = {
        .m00 = 1,
        .m01 = 0,
        .m02 = 0,
        .m03 = 0,

        .m10 = 0,
        .m11 = 1,
        .m12 = 0,
        .m13 = 0,

        .m20 = 0,
        .m21 = 0,
        .m22 = 1,
        .m23 = 0,

        .m30 = 0,
        .m31 = 0,
        .m32 = 0,
        .m33 = 1
};

union vec4 quat_identity = {
        .x = 0,
        .y = 0,
        .z = 0,
        .w = 1
};

union mat3 mat3_identity = {
        .m00 = 1,
        .m01 = 0,
        .m02 = 0,

        .m10 = 0,
        .m11 = 1,
        .m12 = 0,

        .m20 = 0,
        .m21 = 0,
        .m22 = 1
};
