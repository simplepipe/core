#include <kernel/io.h>

#if OS == DROID
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static struct zip *g_assets = NULL;
static char g_local_path[256] = {0};

void file_android_set_assets(struct zip *assets)
{
        g_assets = assets;
}

void file_droid_set_local_directory(const char *path)
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

/*
 * file asset
 */
struct file_droid_asset {
        struct file base;

        struct zip_file *ptr;
};

static struct file_droid_asset *file_droid_asset_new(const char *path);
static struct file_droid_asset *file_droid_asset_init(
        struct file_droid_asset *p);
static void file_droid_asset_release(struct file_droid_asset *p);
static void file_droid_asset_free(const struct ref *ref);
static int file_droid_asset_read(struct file_droid_asset *p, unsigned size,
        void *buf);

static struct file_droid_asset *file_droid_asset_new(const char *path)
{
        struct file_droid_asset *p = file_droid_asset_init(
                malloc(sizeof(struct file_droid_asset)));

        p->ptr = zip_fopen(g_assets, path, 0);
        if(p->ptr) {
                p->base.read = (file_read_delegate) file_droid_asset_read;
        }
        return p;
}

static struct file_droid_asset *file_droid_asset_init(
        struct file_droid_asset *p)
{
        file_init(&p->base);
        p->base.base = (struct ref){file_droid_asset_free, 1};
        p->ptr = NULL;
        return p;
}

static void file_droid_asset_release(struct file_droid_asset *p)
{
        if(p->ptr) zip_fclose(p->ptr);
}

static void file_droid_asset_free(const struct ref *ref)
{
        struct file_droid_asset *p = cast(ref, struct file_droid_asset, base);
        file_droid_asset_release(p);
        free(p);
}

static int file_droid_asset_read(struct file_droid_asset *p, unsigned size,
        void *buf)
{
        return zip_fread(p->ptr, buf, size);
}

/*
 * file local
 */
struct file_droid_local {
        struct file base;

        FILE *ptr;
};

static struct file_droid_local *file_droid_local_new(const char *path, char *mode);
static struct file_droid_local *file_droid_local_init(struct file_droid_local *p);
static void file_droid_local_release(struct file_droid_local *p);
static void file_droid_local_free(const struct ref *ref);
static void file_droid_local_write(struct file_droid_local *p,
        const char *buf, unsigned len);
static int file_droid_local_read(struct file_droid_local *p, unsigned size,
        void *buf);

static struct file_droid_local *file_droid_local_new(const char *path, char *mode)
{
        struct file_droid_local *p = file_droid_local_init(
                malloc(sizeof(struct file_droid_local)));
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
                        file_droid_local_write;
                p->base.read = (file_read_delegate)
                        file_droid_local_read;
        }
        return p;
}

static struct file_droid_local *file_droid_local_init(struct file_droid_local *p)
{
        file_init(&p->base);
        p->base.base = (struct ref){file_droid_local_free, 1};
        p->ptr = NULL;
        return p;
}

static void file_droid_local_release(struct file_droid_local *p)
{
        if(p->ptr) {
                fclose(p->ptr);
        }
}

static void file_droid_local_free(const struct ref *ref)
{
        struct file_droid_local *p = cast(ref, struct file_droid_local, base);
        file_droid_local_release(p);
        free(p);
}

static void file_droid_local_write(struct file_droid_local *p,
        const char *buf, unsigned len)
{
        fwrite(buf, 1, len, p->ptr);
}

static int file_droid_local_read(struct file_droid_local *p, unsigned size,
        void *buf)
{
        return fread(buf, 1, size, p->ptr);
}

struct file *file_open(const char *path, char *mode)
{
        struct file *p = NULL;
        char full_path[256];

        if(strncmp(path, "inner://", sizeof("inner://") - 1) == 0) {
                full_path[0] = '\0';
                strcat(full_path, "assets/");
                strcat(full_path, path + sizeof("inner://") - 1);

                p = &file_droid_asset_new(full_path)->base;
        } else if(strncmp(path, "local://", sizeof("local://") - 1) == 0) {
                full_path[0] = '\0';
                strcat(full_path, g_local_path);
                strcat(full_path, path + sizeof("local://") - 1);

                p = &file_droid_local_new(full_path, mode)->base;
        }

        return p;
}


#endif
