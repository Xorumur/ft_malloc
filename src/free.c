#include "../include/malloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

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
	if (!ptr || is_managed_pointer(ptr))
		return;

	t_block *block = ((t_block *)ptr) - 1;

	pthread_mutex_lock(&g_malloc.lock);

	if (!is_valid_block(block))
	{
		pthread_mutex_unlock(&g_malloc.lock);
		return;
	}

	block->is_free = 1;

	// Si bloc appartient à LARGE → unmap
	t_zone *zone = g_malloc.large;
	while (zone)
	{
		if (zone->blocks == block)
		{
			pthread_mutex_unlock(&g_malloc.lock);
			free_large(ptr);
			return;
		}
		zone = zone->next;
	}

	pthread_mutex_unlock(&g_malloc.lock);
}
