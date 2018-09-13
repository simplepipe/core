#include <stdlib.h>
#include <pthread.h>
#include <kernel/thread.h>

struct job {
        struct list_head head_pool;

        void(*work)(const struct ref *);
        const struct ref *ref;
};

static struct job *__job_new(const struct ref *ref,
        void(*work)(const struct ref *))
{
        struct job *p = malloc(sizeof(struct job));
        list_head_init(&p->head_pool);
        p->work = work;
        p->ref = ref;
        if(ref) ref_inc(ref);
        return p;
}

static void __job_free(struct job *p)
{
        list_del(&p->head_pool);
        if(p->ref) ref_dec(p->ref);
        free(p);
}

struct worker {
        struct thread_pool *pool;
        struct list_head head_pool;

        pthread_mutex_t lock;
        pthread_cond_t cond;
};

struct worker *__worker_new(struct thread_pool *pool);
static void __worker_free(struct worker *p);
static void *__worker_run(struct worker *p);

static void *__worker_run(struct worker *p)
{
        struct thread_pool *pool = p->pool;
        struct job *job = NULL;

#define check_pool() \
        spin_lock(&pool->lock);\
        if(pool->base.count <= 0) {\
                spin_unlock(&pool->lock);\
                goto finish;\
        }\
        spin_unlock(&pool->lock);

get_job:
        check_pool();

        job = NULL;
        spin_lock(&pool->lock);
        if(!list_singular(&pool->works)) {
                job = cast(pool->works.next, struct job, head_pool);
                list_del_init(&job->head_pool);
        }
        spin_unlock(&pool->lock);

        if(!job) goto wait;

        check_pool();

        if(job->ref && job->ref->count == 1) {
                goto end_job;
        }
        if(job->work) {
                job->work(job->ref);
        }
end_job:
        __job_free(job);
        goto get_job;

wait:
        check_pool();

        spin_lock(&pool->lock);
        list_add(&p->head_pool, &pool->workers);
        spin_unlock(&pool->lock);

        pthread_mutex_lock(&p->lock);
        if(!list_singular(&p->head_pool)) {
                pthread_cond_wait(&p->cond, &p->lock);
        }
        pthread_mutex_unlock(&p->lock);
        goto get_job;

finish:
        spin_lock(&pool->lock);
        list_del_init(&p->head_pool);
        spin_unlock(&pool->lock);

        ref_dec(&pool->live);
        __worker_free(p);

        return (void *)1;
}

struct worker *__worker_new(struct thread_pool *pool)
{
        pthread_t tid;

        struct worker *p = malloc(sizeof(struct worker));
        list_head_init(&p->head_pool);
        p->pool = pool;

        pthread_mutex_init(&p->lock, NULL);
        pthread_cond_init(&p->cond, NULL);

        ref_inc(&pool->live);
        pthread_create(&tid, NULL, (void*(*)(void*))__worker_run,
                (void*)p);
        pthread_detach(tid);

        return p;
}

static void __worker_free(struct worker *p)
{
        pthread_mutex_destroy(&p->lock);
        pthread_cond_destroy(&p->cond);
        list_del(&p->head_pool);
        free(p);
}

/*
 * thread pool
 */
static void thread_pool_free(const struct ref *ref);

struct thread_pool *thread_pool_new(unsigned max)
{
        return thread_pool_init(malloc(sizeof(struct thread_pool)), max);
}

struct thread_pool *thread_pool_init(struct thread_pool *p, unsigned max)
{
        unsigned count = max;

        p->base = (struct ref){thread_pool_close, 1};
        p->live = (struct ref){thread_pool_free, 1};
        list_head_init(&p->workers);
        list_head_init(&p->works);
        p->lock = 0;

        spin_lock(&p->lock);
        while(count) {
                __worker_new(p);
                count--;
        }
        spin_unlock(&p->lock);

        return p;
}

void thread_pool_close(const struct ref *ref)
{
        struct thread_pool *pool = cast(ref, struct thread_pool, base);
        struct list_head *head;
        struct worker *worker;

        spin_lock(&pool->lock);
        list_while_not_singular(head, &pool->workers) {
                worker = cast(head, struct worker, head_pool);
                list_del_init(head);

                pthread_mutex_lock(&worker->lock);
                pthread_cond_signal(&worker->cond);
                pthread_mutex_unlock(&worker->lock);
        }
        spin_unlock(&pool->lock);

        ref_dec(&pool->live);
}

void thread_pool_release(struct thread_pool *p)
{
        struct list_head *head;
        struct job *job = NULL;

        spin_lock(&p->lock);
        list_while_not_singular(head, &p->works) {
                job = cast(head, struct job, head_pool);
                __job_free(job);
        }
        spin_unlock(&p->lock);
}

static void thread_pool_free(const struct ref *ref)
{
        struct thread_pool *p = cast(ref, struct thread_pool, live);
        thread_pool_release(p);
        free(p);
}

void thread_pool_add(struct thread_pool *p, const struct ref *ref,
        void(*work)(const struct ref *))
{
        struct job *job = __job_new(ref, work);
        struct worker *worker = NULL;

        spin_lock(&p->lock);

        list_add(&job->head_pool, &p->works);
        if(!list_singular(&p->workers)) {
                worker = cast(p->workers.next, struct worker, head_pool);
                list_del_init(&worker->head_pool);
        }

        spin_unlock(&p->lock);

        if(worker) {
                pthread_mutex_lock(&worker->lock);
                pthread_cond_signal(&worker->cond);
                pthread_mutex_unlock(&worker->lock);
        }
}
