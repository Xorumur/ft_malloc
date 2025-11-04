#include "../include/malloc.h"

size_t align_size(size_t size) {
    return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
}


void print_zones_state(void)
{
    t_zone *z;
    t_block *b;
    int zi = 0, bi;

    printf("=== ZONES STATE ===\n");
    printf("TINY zones:\n");
    for (z = g_malloc.tiny; z; z = z->next, zi++)
    {
        printf(" TINY #%d start=%p size=%zu\n", zi, z->start, z->size);
        bi = 0;
        for (b = z->blocks; b; b = b->next, bi++)
            printf("   block %d: %p size=%zu is_free=%d\n",
                   bi, (void*)(b + 1), b->size, b->is_free);
    }

    printf("SMALL zones:\n");
    zi = 0;
    for (z = g_malloc.small; z; z = z->next, zi++)
    {
        printf(" SMALL #%d start=%p size=%zu\n", zi, z->start, z->size);
        bi = 0;
        for (b = z->blocks; b; b = b->next, bi++)
            printf("   block %d: %p size=%zu is_free=%d\n",
                   bi, (void*)(b + 1), b->size, b->is_free);
    }

    printf("LARGE zones:\n");
    zi = 0;
    for (z = g_malloc.large; z; z = z->next, zi++)
        printf(" LARGE #%d start=%p size=%zu block=%p\n", zi, z->start, z->size, (void*)(z->blocks+1));
    printf("===================\n");
}
