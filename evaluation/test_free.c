#include "../include/malloc.h"
#include <stdio.h>
#include <string.h>

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
