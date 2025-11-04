#include <stdio.h>
#include <string.h>
#include "include/malloc.h"

int main() {
    printf("=== Test de malloc personnalisé ===\n\n");
    
    // Test 1: Allocation tiny
    printf("Test 1: Allocation tiny (50 bytes)\n");
    void *ptr1 = malloc(50);
    if (ptr1) {
        printf("✓ Allocation réussie: %p\n", ptr1);
        strcpy((char*)ptr1, "Hello tiny!");
        printf("✓ Écriture réussie: %s\n", (char*)ptr1);
    } else {
        printf("✗ Échec allocation\n");
    }
    
    // Test 2: Allocation small
    printf("\nTest 2: Allocation small (1000 bytes)\n");
    void *ptr2 = malloc(1000);
    if (ptr2) {
        printf("✓ Allocation réussie: %p\n", ptr2);
        strcpy((char*)ptr2, "Hello small!");
        printf("✓ Écriture réussie: %s\n", (char*)ptr2);
    } else {
        printf("✗ Échec allocation\n");
    }
    
    // Test 3: Allocation large
    printf("\nTest 3: Allocation large (5000 bytes)\n");
    void *ptr3 = malloc(5000);
    if (ptr3) {
        printf("✓ Allocation réussie: %p\n", ptr3);
        strcpy((char*)ptr3, "Hello large!");
        printf("✓ Écriture réussie: %s\n", (char*)ptr3);
    } else {
        printf("✗ Échec allocation\n");
    }
    
    // Test 4: Allocations multiples tiny
    printf("\nTest 4: Allocations multiples tiny (10 x 32 bytes)\n");
    void *ptrs[10];
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(32);
        if (ptrs[i]) {
            sprintf((char*)ptrs[i], "Bloc %d", i);
            printf("✓ Bloc %d: %p - %s\n", i, ptrs[i], (char*)ptrs[i]);
        } else {
            printf("✗ Échec allocation bloc %d\n", i);
        }
    }
    
    // Test 5: Allocation de taille 0
    printf("\nTest 5: Allocation de taille 0\n");
    void *ptr_zero = malloc(0);
    if (ptr_zero == NULL) {
        printf("✓ malloc(0) retourne NULL comme attendu\n");
    } else {
        printf("✗ malloc(0) devrait retourner NULL\n");
    }
    
    printf("\n=== Tests terminés ===\n");
    return 0;
}
