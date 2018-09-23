#ifndef __kernel_string_h
#define __kernel_string_h

#include <stdio.h>
#include <locale.h>
#include <kernel/def.h>
#include <kernel/ref.h>

struct string {
        struct ref base;

        char *ptr;
        unsigned len;

        void*(*realloc)(void*,size_t);
};

struct string *string_new();
struct string *string_init(struct string *p);
void string_release(struct string *p);

struct string *string_cat_chars(struct string *p, const char *s,
        const unsigned s_len);

void string_trim_utf8(struct string *p);

void string_replace(struct string *p, 
	const char *from, const unsigned from_len,
	const char *to, const unsigned to_len);

/*
 * cat
 */
struct string_cat_parameter;
typedef void(*string_cat_parameter_cat)(struct string_cat_parameter *, \
        struct string *);
struct string_cat_parameter {
        string_cat_parameter_cat cat;
};
struct string *string_cat(struct string *p,
        struct string_cat_parameter *param, ...);

/*
 * cat chars
 */
struct string_cat_parameter_chars {
        struct string_cat_parameter base;
        const char *ptr;
        unsigned len;
};

static inline void
string_cat_parameter_chars_cat(struct string_cat_parameter_chars *p,
        struct string *s)
{
        string_cat_chars(s, p->ptr, p->len);
}

#define STRING_CAT_LITERAL(s) (struct string_cat_parameter *)\
&(struct string_cat_parameter_chars){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_chars_cat,\
        .ptr = s,\
        .len = sizeof(s) - 1\
}

#define STRING_CAT_CHARS(s, l) (struct string_cat_parameter *)\
&(struct string_cat_parameter_chars){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_chars_cat,\
        .ptr = s,\
        .len = l\
}

#define STRING_CAT_STRING(s) (struct string_cat_parameter *)\
&(struct string_cat_parameter_chars){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_chars_cat,\
        .ptr = s->ptr,\
        .len = s->len\
}

/*
 * cat i8
 */
struct string_cat_parameter_i8 {
        struct string_cat_parameter base;
        i8 num;
};

static inline void
string_cat_parameter_i8_cat(struct string_cat_parameter_i8 *p,
        struct string *s)
{
        char var[5];
        unsigned len = sprintf(var, "%d", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_I8(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_i8){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_i8_cat,\
        .num = n\
}

/*
 * cat u8
 */
struct string_cat_parameter_u8 {
        struct string_cat_parameter base;
        u8 num;
};

static inline void
string_cat_parameter_u8_cat(struct string_cat_parameter_u8 *p,
        struct string *s)
{
        char var[4];
        unsigned len = sprintf(var, "%u", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_U8(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u8){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u8_cat,\
        .num = n\
}

/*
 * cat i16
 */
struct string_cat_parameter_i16 {
        struct string_cat_parameter base;
        i16 num;
};

static inline void
string_cat_parameter_i16_cat(struct string_cat_parameter_i16 *p,
        struct string *s)
{
        char var[7];
        unsigned len = sprintf(var, "%d", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_I16(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_i16){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_i16_cat,\
        .num = n\
}

/*
 * cat u16
 */
struct string_cat_parameter_u16 {
        struct string_cat_parameter base;
        u16 num;
        unsigned fixed;
};

static inline void
string_cat_parameter_u16_cat(struct string_cat_parameter_u16 *p,
        struct string *s)
{
        char var[6];
        unsigned len = sprintf(var, "%u", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_U16(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u16){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u16_cat,\
        .num = n\
}

static inline void
string_cat_parameter_u16_cat_fixed(struct string_cat_parameter_u16 *p,
        struct string *s)
{
        char var[6];
        char format[10];
        sprintf(format, "%c0%uu", '%', p->fixed);
        unsigned len = sprintf(var, format, p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_U16_FIXED(n, f) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u16){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u16_cat_fixed,\
        .num = n,\
        .fixed = f\
}

/*
 * cat i32
 */
struct string_cat_parameter_i32 {
        struct string_cat_parameter base;
        i32 num;
};

static inline void
string_cat_parameter_i32_cat(struct string_cat_parameter_i32 *p,
        struct string *s)
{
        char var[12];
        unsigned len = sprintf(var, "%d", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_I32(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_i32){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_i32_cat,\
        .num = n\
}

/*
 * cat u32
 */
struct string_cat_parameter_u32 {
        struct string_cat_parameter base;
        u32 num;
};

static inline void
string_cat_parameter_u32_cat(struct string_cat_parameter_u32 *p,
        struct string *s)
{
        char var[11];
        unsigned len = sprintf(var, "%u", p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_U32(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u32){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u32_cat,\
        .num = n\
}

/*
 * cat i64
 */
struct string_cat_parameter_i64 {
        struct string_cat_parameter base;
        i64 num;
};

static inline void
string_cat_parameter_i64_cat(struct string_cat_parameter_i64 *p,
        struct string *s)
{
        char var[21];
        unsigned len = sprintf(var, "%"PRId64 , p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_I64(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_i64){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_i64_cat,\
        .num = n\
}

/*
 * cat u64
 */
struct string_cat_parameter_u64 {
        struct string_cat_parameter base;
        u64 num;
};

static inline void
string_cat_parameter_u64_cat(struct string_cat_parameter_u64 *p,
        struct string *s)
{
        char var[21];
        unsigned len = sprintf(var, "%"PRIu64, p->num);
        string_cat_chars(s, var, len);
}


#define STRING_CAT_U64(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u64){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u64_cat,\
        .num = n\
}

static inline void
string_cat_parameter_u64_cat_dot(struct string_cat_parameter_u64 *p,
        struct string *s)
{
        char var[21];
        setlocale(LC_NUMERIC, "");
        unsigned len = sprintf(var, "%'"PRIu64, p->num);
        string_cat_chars(s, var, len);
}

#define STRING_CAT_U64_DOT(n) (struct string_cat_parameter *)\
&(struct string_cat_parameter_u64){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_u64_cat_dot,\
        .num = n\
}

/*
 * cat double
 */
struct string_cat_parameter_double {
        struct string_cat_parameter base;
        double num;
        unsigned fraction;
};

static inline void
string_cat_parameter_double_cat(struct string_cat_parameter_double *p,
        struct string *s)
{
        char var[128];
        char format[32];
        sprintf(format, "%c.%df", '%', p->fraction);
        unsigned len = sprintf(var, format, p->num);
	string_cat_chars(s, var, len);
}

static inline void
string_cat_parameter_double_or_int_cat(struct string_cat_parameter_double *p,
        struct string *s)
{
        char var[128];
        char format[32];
        if(p->num - (i64)p->num == 0 ) {
                unsigned len = sprintf(var, "%"PRId64, (i64)p->num);
	        string_cat_chars(s, var, len);
        } else {
        	sprintf(format, "%c.%df", '%', p->fraction);
                unsigned len = sprintf(var, format, p->num);
        	string_cat_chars(s, var, len);
        }
}

#define STRING_CAT_DOUBLE(n, f) (struct string_cat_parameter *)\
&(struct string_cat_parameter_double){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_double_cat,\
        .num = n,\
        .fraction = f\
}

#define STRING_CAT_DOUBLE_OR_INT(n, f) (struct string_cat_parameter *)\
&(struct string_cat_parameter_double){\
        .base.cat = (string_cat_parameter_cat) string_cat_parameter_double_or_int_cat,\
        .num = n,\
        .fraction = f\
}

#endif
