#include <kernel/io.h>

#if OS == LINUX || OS == OSX
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct file_pc {
        struct file base;

        FILE *ptr;
};

static struct file_pc *file_pc_new(const char *path, char *mode);
static struct file_pc *file_pc_init(struct file_pc *p);
static void file_pc_release(struct file_pc *p);
static void file_pc_free(const struct ref *ref);
static void file_pc_write(struct file_pc *p,
        const char *buf, const unsigned len);
static int file_pc_read(struct file_pc *p, const unsigned size,
        void *buf);

struct file *file_open(const char *path, char *mode)
{
        struct file *p = NULL;

        if(strncmp(path, "inner://", sizeof("inner://") - 1) == 0) {
                p = &file_pc_new(path + sizeof("inner://") - 1,
                        mode)->base;
        } else if(strncmp(path, "local://", sizeof("local://") - 1) == 0) {
                p = &file_pc_new(path + sizeof("local://") - 1,
                        mode)->base;
        }

        return p;
}

static struct file_pc *file_pc_new(const char *path, char *mode)
{
        struct file_pc *p = file_pc_init(
                malloc(sizeof(struct file_pc)));
        struct stat st = {0};
        char buf[256], *start = strchr(mode, 'w');

        if(!start) {
                start = strchr(mode, 'a');
        }

        if(!start) goto open;

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
                        file_pc_write;
                p->base.read = (file_read_delegate)
                        file_pc_read;
        }
        return p;
}

static struct file_pc *file_pc_init(struct file_pc *p)
{
        file_init(&p->base);
        p->base.base = (struct ref){file_pc_free, 1};
        p->ptr = NULL;
        return p;
}

static void file_pc_release(struct file_pc *p)
{
        if(p->ptr) {
                fclose(p->ptr);
        }
}

static void file_pc_free(const struct ref *ref)
{
        struct file_pc *p = cast(ref, struct file_pc, base);
        file_pc_release(p);
        free(p);
}

static void file_pc_write(struct file_pc *p,
        const char *buf, const unsigned len)
{
        fwrite(buf, 1, len, p->ptr);
}

static int file_pc_read(struct file_pc *p, const unsigned size,
        void *buf)
{
        return fread(buf, 1, size, p->ptr);
}

#endif
