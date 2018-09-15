#include <stdio.h>
#include <collada/float_array.h>

int main(int argc, char **argv)
{
        char buf[] = "+1.1 1.2 1.3 1.4 1.5 1 1 1 1 1 1 1 1   2323";

        struct float_array *p = float_array_new();
        float_array_reserve(p, 14);
        float_array_add_string(p, buf);
        float_array_add_float(p, 222.222f);
        for(unsigned  i = 0; i < p->len; ++i) {
                printf("%.3f ", p->ptr[i]);
        }
        printf("\n");
        ref_dec(&p->base);
        return 0;
}
