#include <stdlib.h>
#include <string.h>
#include <xml/xml.h>

static struct xml_attribute *xml_attribute_init(struct xml_attribute *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);
static struct xml_attribute *xml_attribute_init_mask(struct xml_attribute *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len);

static void __xml_attribute_free(const struct ref *ref);

struct xml_attribute *xml_attribute_new(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        char *ptr;

        struct xml_attribute *p = xml_attribute_init(malloc(
                sizeof(struct xml_attribute) + name_len + value_len + 2),
                name, name_len, value, value_len);

        ptr = (char *)p;
        ptr += sizeof(struct xml_attribute);

        if(name_len) {
                memcpy(ptr, name, name_len);
                ptr += name_len;
        }
        ptr++;

        if(value_len) {
                memcpy(ptr, value, value_len);
        }

        return p;
}

static struct xml_attribute *xml_attribute_init(struct xml_attribute *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        char *ptr;

        p->base = (struct ref){__xml_attribute_free, 1};

        ptr = (char *)p;
        ptr += sizeof(struct xml_attribute);

        p->name = ptr;
        ptr += name_len;
        *ptr = '\0';
        ptr++;

        p->value = ptr;
        ptr += value_len;
        *ptr = '\0';

        *(unsigned *)&p->name_len = name_len;
        *(unsigned *)&p->value_len = value_len;

        return p;
}

struct xml_attribute *xml_attribute_new_mask(
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        return xml_attribute_init_mask(malloc(sizeof(struct xml_attribute)),
                name, name_len, value, value_len);
}

static struct xml_attribute *xml_attribute_init_mask(struct xml_attribute *p,
        const char *name, const unsigned name_len,
        const char *value, const unsigned value_len)
{
        p->base = (struct ref){__xml_attribute_free, 1};
        p->name = name;
        p->value = value;
        *(unsigned *)&p->name_len = name_len;
        *(unsigned *)&p->value_len = value_len;

        return p;
}

void xml_attribute_release(struct xml_attribute *p)
{

}

static void __xml_attribute_free(const struct ref *ref)
{
        struct xml_attribute *p = cast(ref, struct xml_attribute, base);
        xml_attribute_release(p);
        free(p);
}
