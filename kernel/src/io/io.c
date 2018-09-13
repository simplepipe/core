#include <stdlib.h>
#include <kernel/io.h>

struct file *file_init(struct file *p)
{
        p->write = NULL;
        p->read = NULL;
        return p;
}

void file_write(struct file *p, const char *buf, unsigned len)
{
        if(p->write) {
                p->write(p, buf, len);
        }
}

int file_read(struct file *p, unsigned size, void *buf)
{
        if(p->read) {
                return p->read(p, size, buf);
        }
        return 0;
}

struct string *file_to_string(const char *path)
{
        struct string *p = string_new();
        struct file *f = file_open(path, "r");
        char buf[8192];
        int count;

        while((count = file_read(f, 8192, buf))) {
                string_cat(p,
                        STRING_CAT_CHARS(buf, count),
                        NULL
                );
        }

        ref_dec(&f->base);
        return p;
}
