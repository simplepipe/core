#ifndef __kernel_pool_list_h
#define __kernel_pool_list_h

struct pool_list_head {
        struct pool_list_head *prev;
        struct pool_list_head *next;
        struct pool_list_head *pprev;
};

#define POOL_LIST_HEAD_INIT(p) {&(p), &(p), &(p)}

#define pool_list_for_each(p, list) \
        for(p = (list)->next; p != (list); p = p->next)

#define pool_list_back_each(p, list) \
        for(p = (list)->prev; p != (list); p = p->prev)

#define pool_list_while_not_singular(p, list) \
        while(!pool_list_singular(list) && (p = (list)->next))

#define pool_list_for_each_secure(head, list, work)                          \
        {                                                               \
                struct pool_list_head __temp = POOL_LIST_HEAD_INIT(__temp);       \
                pool_list_while_not_singular(head, list) {                   \
                        pool_list_del(head);                                 \
                        pool_list_add(head, &__temp);                   \
                        work                                            \
                }                                                       \
                pool_list_while_not_singular(head, &__temp) {                \
                        pool_list_del(head);                                 \
                        pool_list_add(head, list);                      \
                }                                                       \
        }


static inline void pool_list_head_init(struct pool_list_head *list)
{
        list->prev = list;
        list->next = list;
        list->pprev = list;
}

static inline void __pool_list_add(struct pool_list_head *newp,
                              struct pool_list_head *prev,
                              struct pool_list_head *next)
{
        next->prev = newp;
        newp->next = next;
        newp->prev = prev;
        prev->next = newp;
        newp->pprev = prev->pprev;
}

static inline void pool_list_add(struct pool_list_head *newp,
        struct pool_list_head *head)
{
        __pool_list_add(newp, head->prev, head);
}

static inline void pool_list_add_first(struct pool_list_head *newp,
        struct pool_list_head *head)
{
        __pool_list_add(newp, head, head->next);
}

static inline struct pool_list_head *pool_list_get(
        struct pool_list_head *head)
{
        struct pool_list_head *first = head->pprev;
        struct pool_list_head *next = first->next;
        first->next = next->next;
        next->next->prev = first;
        next->next = next;
        next->prev = next;
        return next;
}

static inline void pool_list_del(struct pool_list_head *head)
{
        struct pool_list_head *first = head->prev;
        struct pool_list_head *next = head->next;
        first->next = next;
        next->prev = first;
        head->next = head;
        head->prev = head;
        head->pprev = head;
}

static inline int pool_list_item_singular(struct pool_list_head *head)
{
        return head == head->next;
}

static inline int pool_list_item_has_pool(struct pool_list_head *head)
{
        return head != head->pprev;
}

static inline int pool_list_singular(struct pool_list_head *head)
{
        struct pool_list_head *first = head->pprev;
        return first == first->next;
}

static inline void pool_list_clear(struct pool_list_head *list)
{
        struct pool_list_head *plhead;

        pool_list_while_not_singular(plhead, list) {
                pool_list_del(plhead);
        }
}

#endif
