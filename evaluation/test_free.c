#include "../include/malloc.h"
#include <stdio.h>
#include <string.h>

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

/**
 * Petit test pour la qualité du free
 */
int main(void)
{
    printf("===== TEST FREE QUALITY =====\n");

    printf("[1] Allocation de 10 blocs de 1Ko (TINY)...\n");
    void *tiny[10];
    for (int i = 0; i < 10; i++)
        tiny[i] = malloc(1024);

    printf("[2] Allocation de 5 blocs de 50Ko (SMALL)...\n");
    void *small[5];
    for (int i = 0; i < 5; i++)
        small[i] = malloc(50 * 1024);

    printf("\n--- AVANT FREE ---\n");
    print_zones_state();

    printf("\n[3] Libération de tous les blocs...\n");
    for (int i = 0; i < 10; i++)
        free(tiny[i]);
    for (int i = 0; i < 5; i++)
        free(small[i]);

    printf("\n--- APRÈS FREE ---\n");
    print_zones_state();

    printf("===== FIN DU TEST =====\n");
    return 0;
}
