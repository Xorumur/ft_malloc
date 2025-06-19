// #include "../include/malloc.h"
// #include <stdio.h>
// #include <string.h>

// #define ALIGN(x) (((x) + 15) & ~15)
// #define BLOCK_SIZE sizeof(t_block)

// t_malloc_env g_malloc = {
// 	.tiny = NULL,
// 	.small = NULL,
// 	.large = NULL,
// 	.lock = PTHREAD_MUTEX_INITIALIZER
// };

// static void	*add_large_block(size_t size)
// {
// 	size_t total_size = ALIGN(size) + BLOCK_SIZE + sizeof(t_zone);
// 	t_zone *zone = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
// 						MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
// 	if (zone == MAP_FAILED)
// 		return NULL;

// 	dprintf(2, "[malloc] mmaped LARGE zone at %p (%zu bytes)\n", zone, total_size);

// 	zone->start = zone;
// 	zone->size = total_size;

// 	t_block *block = (t_block *)(zone + 1);
// 	block->size = ALIGN(size);
// 	block->is_free = 0;
// 	block->next = NULL;

// 	zone->blocks = block;
// 	zone->next = g_malloc.large;
// 	g_malloc.large = zone;

// 	dprintf(2, "[malloc] added LARGE block %p (user ptr = %p)\n", (void *)block, (void *)(block + 1));
// 	return (void *)(block + 1);
// }

// void	*malloc(size_t size)
// {
// 	void *ptr = NULL;

// 	if (size == 0)
// 		return NULL;

// 	pthread_mutex_lock(&g_malloc.lock);

// 	dprintf(2, "[malloc] Requested size: %zu\n", size);

// 	if (size > SMALL_LIMIT)
// 		ptr = add_large_block(size);
// 	else
// 	{
// 		// Ici tu dois appeler une future fonction tiny_or_small_alloc(size)
// 		dprintf(2, "[malloc] Requested size is TINY/SMALL - not implemented\n");
// 		ptr = NULL;
// 	}

// 	pthread_mutex_unlock(&g_malloc.lock);

// 	dprintf(2, "[malloc] Returning ptr: %p\n", ptr);
// 	return ptr;
// }

#include "../include/malloc.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define ALIGN(x) (((x) + 15) & ~15)
#define BLOCK_SIZE sizeof(t_block)

t_malloc_env g_malloc = {
	.tiny = NULL,
	.small = NULL,
	.large = NULL,
	.lock = PTHREAD_MUTEX_INITIALIZER
};

/**
 * Petit logger sécurisé pour afficher un message avec un pointeur ou une taille.
 */
static void	log_ptr(const char *msg, void *ptr)
{
	char buffer[128];
	int len = snprintf(buffer, sizeof(buffer), "[log] %s: %p\n", msg, ptr);
	write(2, buffer, len);
}

static void	log_size(const char *msg, size_t size)
{
	char buffer[128];
	int len = snprintf(buffer, sizeof(buffer), "[log] %s: %zu\n", msg, size);
	write(2, buffer, len);
}

static void	*allocate_in_zone(t_zone **zone_list, size_t size, size_t zone_size)
{
	t_zone *zone = *zone_list;

	while (zone)
	{
		t_block *block = zone->blocks;
		while (block)
		{
			if (block->is_free && block->size >= size)
			{
				block->is_free = 0;
				return (void *)(block + 1);
			}
			block = block->next;
		}
		zone = zone->next;
	}

	// Aucun bloc libre, on mmap une nouvelle zone
	void *mem = mmap(NULL, zone_size, PROT_READ | PROT_WRITE,
					 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (mem == MAP_FAILED)
		return NULL;

	t_zone *new_zone = (t_zone *)mem;
	new_zone->start = new_zone;
	new_zone->size = zone_size;
	new_zone->next = *zone_list;
	*zone_list = new_zone;

	t_block *block = (t_block *)(new_zone + 1);
	block->size = ALIGN(size);
	block->is_free = 0;
	block->next = NULL;

	new_zone->blocks = block;
	return (void *)(block + 1);
}


static void	*add_large_block(size_t size)
{
	size_t total_size = ALIGN(size) + BLOCK_SIZE + sizeof(t_zone);
	t_zone *zone = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
						MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (zone == MAP_FAILED)
		return NULL;

	log_ptr("mmaped LARGE zone", zone);
	log_size("mmaped LARGE total size", total_size);

	zone->start = zone;
	zone->size = total_size;

	t_block *block = (t_block *)(zone + 1);
	block->size = ALIGN(size);
	block->is_free = 0;
	block->next = NULL;

	zone->blocks = block;
	zone->next = g_malloc.large;
	g_malloc.large = zone;

	log_ptr("added LARGE block", block);
	log_ptr("user pointer", (void *)(block + 1));

	return (void *)(block + 1);
}

void	*malloc(size_t size)
{
	void *ptr = NULL;

	if (size == 0)
		return NULL;

	pthread_mutex_lock(&g_malloc.lock);

	log_size("Requested size", size);

	if (size > SMALL_LIMIT)
		ptr = add_large_block(size);
	else if (size <= TINY_LIMIT)
		ptr = allocate_in_zone(&g_malloc.tiny, size, TINY_ZONE_SIZE);
	else
		ptr = allocate_in_zone(&g_malloc.small, size, SMALL_ZONE_SIZE);


	pthread_mutex_unlock(&g_malloc.lock);

	log_ptr("Returning ptr", ptr);
	return ptr;
}
