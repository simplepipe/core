#include <kernel/io.h>
#include <action/manager.h>
#include <action/action_sequence.h>
#include <action/action_parallel.h>
#include <action/action_ease.h>
#include <action/action_lerp.h>

static void __callback(const struct ref *ref)
{
        printf("here\n");
        struct action_manager *p = cast(ref, struct action_manager, base);
        action_manager_clear(p);
}

int main(int argc, char **argv)
{
        struct action_manager *p = action_manager_new();

        float test[4] = {2, 3, 4, 5};
        float amount[4] = {1, 2, 3, 4};

        struct action *a = &action_lerp_new(
                4, test, amount, NULL, 10.0f, 0.01f
        )->base;

        action_manager_add_action(p, a);

        action_manager_update(p, 1.0f / 60);
        action_manager_update(p, 1.0f / 60);
        action_manager_update(p, 1.0f / 60);
        printf("%f %f %f %f\n", test[0], test[1], test[2], test[3]);
        action_manager_clear(p);

        ref_dec(&p->base);
        return 0;
}
