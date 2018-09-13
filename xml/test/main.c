#include <kernel/io.h>
#include <kernel/string.h>
#include <xml/xml.h>

int main(int argc, char **argv)
{
        struct xml_context *p = xml_context_new();
        xml_context_parse(p, "inner://res/test.xml");
        struct array *result = array_new(1);
        if(p->root) {
                xml_node_search(p->root, "//*[@name=\"em\" @detail=\"check\"]/../..", result);
                printf("%d\n", result->len);
                printf("%s\n", p->root->name);
        }
        ref_dec(&result->base);
        ref_dec(&p->base);
        return 0;
}
