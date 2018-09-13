#ifndef __kernel_io_h
#define __kernel_io_h

#include <stdio.h>
#include <stdarg.h>
#include <kernel/ref.h>
#include <kernel/platform.h>
#include <kernel/string.h>

#if OS == DROID
        #include <android/log.h>
        #include <zip/zip.h>
#endif

#ifdef NDEBUG
        #define debug(...)
#else
        #if OS == DROID
                #define debug(...) \
                __android_log_print(ANDROID_LOG_DEBUG, \
                        "app_debug",__VA_ARGS__)
        #else
                #define debug(...) printf(__VA_ARGS__)
        #endif
#endif

struct file;

typedef void(*file_write_delegate)(struct file *, const char *, const unsigned);
typedef int(*file_read_delegate)(struct file *, const unsigned, void *);

struct file {
        struct ref base;

        file_write_delegate     write;
        file_read_delegate      read;
};

struct file *file_init(struct file *p);
struct file *file_open(const char *path, char *mode);
void file_write(struct file *p, const char *buf, const unsigned len);
int file_read(struct file *p, const unsigned size, void * const buf);
struct string *file_to_string(const char *path);
unsigned file_to_buffer(const char *path, void * const buf, 
        const unsigned buf_len);

#if OS == WEB
void file_web_set_local_path(const char *path);
#endif

#if OS == DROID
void file_droid_set_assets(struct zip *assets);
void file_droid_set_local_directory(const char *path);
#endif

#endif
