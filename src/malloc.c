#include "../include/malloc.h"

t_malloc g_malloc = {
    .tiny = NULL,
    .small = NULL,
    .large = NULL,
    .lock = PTHREAD_MUTEX_INITIALIZER
};

t_zone **get_zone_list(size_t size) {
    if (size <= (size_t)TINY_LIMIT) {
        return &g_malloc.tiny;
    } else if (size <= (size_t)SMALL_LIMIT) {
        return &g_malloc.small;
    } else {
        return &g_malloc.large;
    }
}

size_t get_zone_size(size_t size) {
    if (size <= (size_t)TINY_LIMIT) {
        return TINY_ZONE_SIZE;
    } else if (size <= (size_t)SMALL_LIMIT) {
        return SMALL_ZONE_SIZE;
    } else {
        return sizeof(t_zone) + sizeof(t_block) + align_size(size); // For large allocations, use the requested size
    }
}

void *malloc(size_t size) {
    void *ptr = NULL;

    if (size == 0) {
        return NULL;
    }

    size_t aligned_size = align_size(size);

    pthread_mutex_lock(&g_malloc.lock);
    
    t_zone **zone_list = get_zone_list(aligned_size); // Trouver la liste de zones appropriée
    size_t zone_size = get_zone_size(aligned_size); // Taille de la zone à allouer

    t_block *blk = NULL;
    
    // Chercher un bloc libre dans toutes les zones existantes
    if (*zone_list != NULL) {
        t_zone *current_zone = *zone_list;
        while (current_zone && !blk) {
            blk = find_free_block(current_zone, aligned_size);
            current_zone = current_zone->next;
        }
    }
    
    // Si aucun bloc libre n'est trouvé, créer une nouvelle zone
    if (!blk) {
        t_zone *new_zone = create_zone(zone_size);
        if (!new_zone) {
            pthread_mutex_unlock(&g_malloc.lock);
            return NULL;
        }
        init_zone(new_zone, zone_size);
        new_zone->next = *zone_list;
        *zone_list = new_zone;
        
        // Prendre le premier bloc de la nouvelle zone
        blk = new_zone->blocks;
    }
    
    // Marquer le bloc comme occupé et le splitter si nécessaire
    if (blk) {
        blk->is_free = 0;
        split_block(blk, aligned_size);
        ptr = (void *)(blk + 1); // Retourner l'adresse après l'en-tête
    }

    pthread_mutex_unlock(&g_malloc.lock);
    return ptr;
}