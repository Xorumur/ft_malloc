#include "../include/malloc.h"

t_block *find_free_block(t_zone *zone, size_t size) {
    t_block *current = zone->blocks;
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

t_block *find_block(t_zone **zone, void *ptr) {
    if (!zone || !*zone) 
        return NULL;
    t_block *current = (*zone)->blocks;
    while (current) {
        if ((void *)(current + 1) == ptr) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void split_block(t_block *blk, size_t size) {
    if (blk->size >= size + sizeof(t_block)) {
        t_block *new_blk = (t_block *)((char *)(blk + 1) + size);
        new_blk->size = blk->size - size - sizeof(t_block);
        new_blk->is_free = 1;
        new_blk->next = blk->next;
        blk->next = new_blk;
        blk->size = size;
    }
}

t_zone *create_zone(size_t size) {
    t_zone *zone = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (zone == MAP_FAILED) {
        return NULL;
    }
    zone->size = size;
    zone->blocks = NULL;
    zone->next = NULL;
    zone->start = zone; // Store the start address of the zone
    return zone;
}

void init_zone(t_zone *zone, size_t size) {
    t_block *blk = (t_block *)(zone + 1);
    blk->size = size - sizeof(t_zone);
    blk->is_free = 1;
    blk->next = NULL;
    zone->blocks = blk;
}