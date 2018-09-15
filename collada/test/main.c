#include <stdio.h>
#include <collada/context.h>

int main(int argc, char **argv)
{
        struct collada_context *ctx = collada_context_new();
        collada_context_parse(ctx, "inner://res/skin.dae");
        ref_dec(&ctx->base);
        return 0;
}
