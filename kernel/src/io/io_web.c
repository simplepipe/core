#include <kernel/io.h>

#if OS == WEB

static char g_local_path[256] = {0};

void file_web_set_local_path(const char *path)
{
        unsigned len = sprintf(g_local_path, "%s", path);
        if(len) {
                if(g_local_path[len - 1] != '/') {
                        g_local_path[len] = '/';
                        len++;
                        g_local_path[len] = '\0';
                }
        }
}

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <emscripten/emscripten.h>

struct file_web {
        struct file base;
        unsigned read_only: 1;
        FILE *ptr;
};

static struct file_web *file_web_new(const char *path, char *mode);
static struct file_web *file_web_init(struct file_web *p);
static void file_web_release(struct file_web *p);
static void file_web_free(const struct ref *ref);
static void file_web_write(struct file_web *p,
        const char *buf, const unsigned len);
static int file_web_read(struct file_web *p, const unsigned size,
        void *buf);

struct file *file_open(const char *path, char *mode)
{
        struct file *p = NULL;
        char full_path[256];

        if(strncmp(path, "inner://", sizeof("inner://") - 1) == 0) {
                p = &file_web_new(path + sizeof("inner://") - 1, mode)->base;
        } else if(strncmp(path, "local://", sizeof("local://") - 1) == 0) {
                full_path[0] = '\0';
                strcat(full_path, g_local_path);
                strcat(full_path, path + sizeof("local://") - 1);
                p = &file_web_new(full_path, mode)->base;
        }

        return p;
}

static struct file_web *file_web_new(const char *path, char *mode)
{
        struct file_web *p = file_web_init(
                malloc(sizeof(struct file_web)));
        struct stat st = {0};
        char buf[256], *start = strchr(mode, 'w');

        if(!start) {
                start = strchr(mode, 'a');
        }

        if(!start) goto open;

        p->read_only = 0;
        sprintf(buf, "%s", path);
        start = buf;
        while(*start) {
                if(*start == '/' && start > buf) {
                        *start = '\0';
                        if(stat(buf, &st) == -1) {
                                mkdir(buf, 0700);
                        }
                        *start = '/';
                }

                start++;
        }

open:
        p->ptr = fopen(path, mode);
        if(p->ptr) {
                p->base.write = (file_write_delegate)
                        file_web_write;
                p->base.read = (file_read_delegate)
                        file_web_read;
        } else {
                p->read_only = 1;
        }
        return p;
}

static struct file_web *file_web_init(struct file_web *p)
{
        file_init(&p->base);
        p->base.base = (struct ref){file_web_free, 1};
        p->ptr = NULL;
        p->read_only = 1;
        return p;
}

static void file_web_release(struct file_web *p)
{
        if(p->ptr) {
                fclose(p->ptr);
        }
        if(p->read_only == 0) {
                EM_ASM(
                        FS.syncfs(false, function (err) {

                        });
                );
        }
}

static void file_web_free(const struct ref *ref)
{
        struct file_web *p = cast(ref, struct file_web, base);
        file_web_release(p);
        free(p);
}

static void file_web_write(struct file_web *p,
        const char *buf, const unsigned len)
{
        fwrite(buf, 1, len, p->ptr);
}

static int file_web_read(struct file_web *p, const unsigned size,
        void *buf)
{
        return fread(buf, 1, size, p->ptr);
}

#endif
