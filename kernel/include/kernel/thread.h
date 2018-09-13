#ifndef __kernel_thread_h
#define __kernel_thread_h

#include <kernel/ref.h>
#include <kernel/list.h>

static inline void
spin_lock(volatile unsigned *lock)
{
        while(__sync_lock_test_and_set(lock, 1)) {
                while(*lock);
        }
}

static inline void
spin_unlock(volatile unsigned *lock)
{
        __sync_lock_release(lock);
}

struct thread_pool {
        struct ref base;
        struct ref live;

        struct list_head workers;
        struct list_head works;
        volatile unsigned lock;
};

struct thread_pool *thread_pool_new(unsigned max);
struct thread_pool *thread_pool_init(struct thread_pool *p, unsigned max);

void thread_pool_release(struct thread_pool *p);
void thread_pool_close(const struct ref *ref);

void thread_pool_add(struct thread_pool *p, const struct ref *ref,
        void(*work)(const struct ref *));

#endif
