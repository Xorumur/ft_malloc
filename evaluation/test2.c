#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/malloc.h"
#include <stdio.h>


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