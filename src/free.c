#include "../include/malloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

int	is_managed_pointer(void *ptr)
{
	t_zone *zone;
	t_block *block;

	for (zone = g_malloc.tiny; zone; zone = zone->next)
		for (block = zone->blocks; block; block = block->next)
			if ((void *)(block + 1) == ptr)
				return 1;

	for (zone = g_malloc.small; zone; zone = zone->next)
		for (block = zone->blocks; block; block = block->next)
			if ((void *)(block + 1) == ptr)
				return 1;

	for (zone = g_malloc.large; zone; zone = zone->next)
		if ((void *)(zone->blocks + 1) == ptr)
			return 1;

	return 0;
}

/**
 * Vérifie qu'un bloc est valide.
 */
static int is_valid_block(t_block *block)
{
	return block && block->size > 0;
}

/**
 * Libère une zone LARGE avec munmap.
 */
static void	free_large(void *ptr)
{
	t_zone *prev = NULL;
	t_zone *zone = g_malloc.large;

	while (zone)
	{
		if ((void *)(zone->blocks + 1) == ptr)
		{
			if (prev)
				prev->next = zone->next;
			else
				g_malloc.large = zone->next;

			munmap(zone->start, zone->size);
			return;
		}
		prev = zone;
		zone = zone->next;
	}
}

/**
 * Fonction free principale.
 */
void	free(void *ptr)
{
	if (!ptr)
		return;

	if (!is_managed_pointer(ptr)) {
		// printf("[DEBUG] invalid pointer\n");
		return;
	}


	t_block *block = ((t_block *)ptr) - 1;

	// printf("[DEBUG] free: ptr=%p, block=%p, size=%zu\n", ptr, block, block->size);

	if (!is_valid_block(block))
	{
		// printf("[DEBUG] block not valid\n");
		return;
	}

	if (find_block(&g_malloc.tiny, block) || find_block(&g_malloc.small, block)) {
		// Si le bloc appartient à TINY ou SMALL, on le marque comme libre
		// printf("[DEBUG] block found in TINY/SMALL, marking as free\n");
		block->is_free = 1;
		if (find_block(&g_malloc.tiny, block))
			try_unmap_zone(&g_malloc.tiny);
		else if (find_block(&g_malloc.small, block))
			try_unmap_zone(&g_malloc.small);
		return;
	}

	// printf("[DEBUG] block not found in TINY/SMALL, marking as free anyway\n");
	block->is_free = 1;

	// Si bloc appartient à LARGE → unmap
	t_zone *zone = g_malloc.large;
	while (zone)
	{
		if (zone->blocks == block)
		{
			// printf("[DEBUG] block is LARGE, unmapping\n");
			free_large(ptr);
			return;
		}
		zone = zone->next;
	}

}
