#include <stdio.h>
#include <collada/context.h>

int main(int argc, char **argv)
{
        struct dae_context *ctx = dae_context_new();
        dae_context_parse(ctx, "inner://res/cube.dae");
        ref_dec(&ctx->base);
        return 0;
}
