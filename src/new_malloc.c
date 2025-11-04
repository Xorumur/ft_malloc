#include "../include/malloc.h"
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define ALIGN(x) (((x) + 15) & ~15)
#define BLOCK_SIZE sizeof(t_block)

t_malloc_env g_malloc = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.lock = PTHREAD_MUTEX_INITIALIZER
};

void *malloc(size_t size) {
    void *ptr = NULL;

    if (size == 0)
        return ptr;
    
	pthread_mutex_lock(&g_malloc.lock);

    if (size < (size_t)TINY_LIMIT) {
        t_zone **zone_list = &g_malloc.tiny;


    } else if (size < (size_t)SMALL_LIMIT) {
        t_zone **zone_list = &g_malloc.small;
        t_block *blk = find_free_block(*zone_list, size);

    } else {
        t_zone **zone_list = &g_malloc.large;

    }
}