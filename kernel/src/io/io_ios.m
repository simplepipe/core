#import <kernel/io.h>

#if OS == IOS

#import <Foundation/Foundation.h>
#import <stdlib.h>
#import <string.h>
#import <sys/stat.h>

struct file_ios {
        struct file base;

        FILE *ptr;
};

static struct file_ios *file_ios_new(const char *path, char *mode);
static struct file_ios *file_ios_init(struct file_ios *p);
static void file_ios_release(struct file_ios *p);
static void file_ios_free(const struct ref *ref);
static void file_ios_write(struct file_ios *p,
        const char *buf, unsigned len);
static int file_ios_read(struct file_ios *p, unsigned size,
        void *buf);
static NSString *__path_inner(const char *path);
static NSString * __path_local(const char *path);

static NSString *__path_inner(const char *path)
{
        return [[NSBundle mainBundle]
                pathForResource:[NSString stringWithUTF8String:path]
                ofType:nil inDirectory:nil];
}

static NSString * __path_local(const char *path)
{
        NSArray *paths = NSSearchPathForDirectoriesInDomains
            (NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *dir = [paths objectAtIndex:0];
        return [NSString stringWithFormat:@"%@/%s", dir, path];
}

struct file *file_open(const char *path, char *mode)
{
        struct file *p = NULL;
        NSString *full = nil;

        if(strncmp(path, "inner://", sizeof("inner://") - 1) == 0) {
                full = __path_inner(path + sizeof("inner://") - 1);
        } else if(strncmp(path, "local://", sizeof("local://") - 1) == 0) {
                full = __path_local(path + sizeof("local://") - 1);
        }

        if(full) p = &file_ios_new([full UTF8String], mode)->base;

        full = nil;
        return p;
}

static struct file_ios *file_ios_new(const char *path, char *mode)
{
        struct file_ios *p = file_ios_init(
                malloc(sizeof(struct file_ios)));
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
                        file_ios_write;
                p->base.read = (file_read_delegate)
                        file_ios_read;
        }
        return p;
}

static struct file_ios *file_ios_init(struct file_ios *p)
{
        file_init(&p->base);
        p->base.base = (struct ref){file_ios_free, 1};
        p->ptr = NULL;
        return p;
}

static void file_ios_release(struct file_ios *p)
{
        if(p->ptr) {
                fclose(p->ptr);
        }
}

static void file_ios_free(const struct ref *ref)
{
        struct file_ios *p = cast(ref, struct file_ios, base);
        file_ios_release(p);
        free(p);
}

static void file_ios_write(struct file_ios *p,
        const char *buf, unsigned len)
{
        fwrite(buf, 1, len, p->ptr);
}

static int file_ios_read(struct file_ios *p, unsigned size,
        void *buf)
{
        return fread(buf, 1, size, p->ptr);
}

#endif
