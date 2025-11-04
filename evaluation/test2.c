#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/malloc.h"
#include <stdio.h>

void print_zones_state(void)
{
    t_zone *z;
    t_block *b;
    int zi = 0, bi;

    printf("\n=== ZONES STATE ===\n");

    printf("TINY zones:\n");
    for (z = g_malloc.tiny; z; z = z->next, zi++)
    {
        printf(" TINY #%d start=%p size=%zu\n", zi, z->start, z->size);
        bi = 0;
        for (b = z->blocks; b; b = b->next, bi++)
            printf("   block %d: %p size=%zu is_free=%d\n",
                   bi, (void *)(b + 1), b->size, b->is_free);
    }

    printf("SMALL zones:\n");
    zi = 0;
    for (z = g_malloc.small; z; z = z->next, zi++)
    {
        printf(" SMALL #%d start=%p size=%zu\n", zi, z->start, z->size);
        bi = 0;
        for (b = z->blocks; b; b = b->next, bi++)
            printf("   block %d: %p size=%zu is_free=%d\n",
                   bi, (void *)(b + 1), b->size, b->is_free);
    }

    printf("LARGE zones:\n");
    zi = 0;
    for (z = g_malloc.large; z; z = z->next, zi++)
        printf(" LARGE #%d start=%p size=%zu block=%p\n",
               zi, z->start, z->size, (void *)(z->blocks + 1));

    printf("===================\n");
}

void    print(char *s)
{
    write(1, s, strlen(s));
}

int main(void)
{
    int   i;
    char  *addr;

    print("===== TEST LOOP ALLOC/FREE =====\n");

    i = 0;
    while (i < 10) // tu peux augmenter à 1024 ensuite
    {
        addr = (char *)malloc(1024); // TINY allocation
        if (addr == NULL)
        {
            print("Failed to allocate memory\n");
            return (1);
        }
        addr[0] = 42;

        printf("\n[ITERATION %d] Après malloc:\n", i);
        print_zones_state();

        free(addr);

        printf("[ITERATION %d] Après free:\n", i);
        print_zones_state();

        i++;
    }

    print("\n===== FIN DU TEST =====\n");
    return (0);
}