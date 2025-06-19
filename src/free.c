// #include "../include/malloc.h"
// #include <stdio.h>
// #include <sys/mman.h>

// static int is_valid_block(t_block *block)
// {
// 	// Simple vérification de structure (peut être améliorée)
// 	return block && block->size > 0;
// }

// static void	free_large(void *ptr)
// {
// 	t_zone *prev = NULL;
// 	t_zone *zone = g_malloc.large;

// 	while (zone)
// 	{
// 		dprintf(2, "[free_large] zone: %p, block: %p\n", (void *)zone, (void *)zone->blocks);
// 		if ((void *)(zone->blocks + 1) == ptr)
// 		{
// 			dprintf(2, "[free_large] unmapping %p (size %zu)\n", zone->start, zone->size);
// 			if (prev)
// 				prev->next = zone->next;
// 			else
// 				g_malloc.large = zone->next;
// 			munmap(zone->start, zone->size);
// 			return;
// 		}
// 		prev = zone;
// 		zone = zone->next;
// 	}
// }


// void	free(void *ptr)
// {
// 	if (!ptr)
// 		return;

// 	t_block *block = ((t_block *)ptr) - 1;

// 	pthread_mutex_lock(&g_malloc.lock);

// 	if (!is_valid_block(block))
// 	{
// 		pthread_mutex_unlock(&g_malloc.lock);
// 		return;
// 	}

// 	block->is_free = 1;

// 	// Cas LARGE : on libère toute la zone
// 	t_zone *zone = g_malloc.large;
// 	while (zone)
// 	{
// 		dprintf(2, "zone: %p, block: %p\n", (void *)zone, (void *)zone->blocks);

// 		if (zone->blocks == block)
// 		{
// 			dprintf(2, "==> Found LARGE block to munmap: %p\n", ptr);
// 			pthread_mutex_unlock(&g_malloc.lock);
// 			free_large(ptr);
// 			return;
// 		}
// 		zone = zone->next;
// 	}

// 	pthread_mutex_unlock(&g_malloc.lock);
// }

#include "../include/malloc.h"
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

/**
 * Écrit un message suivi de deux pointeurs en hexadécimal.
 * Utilise write, pas printf ni dprintf.
 */
static void	log_ptr_msg(const char *msg, void *ptr1, void *ptr2)
{
	char buffer[256];
	int len = 0;

	len += snprintf(buffer + len, sizeof(buffer) - len,
		"[log] %s: %p %p\n", msg, ptr1, ptr2);
	write(2, buffer, len);
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
		log_ptr_msg("free_large zone/block", zone, zone->blocks);
		if ((void *)(zone->blocks + 1) == ptr)
		{
			// Petit log sûr :
			write(2, "[free_large] unmapping\n", 24);

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
		log_ptr_msg("free check zone/block", zone, zone->blocks);

		if (zone->blocks == block)
		{
			write(2, "==> LARGE block: unmap\n", 24);
			pthread_mutex_unlock(&g_malloc.lock);
			free_large(ptr);
			return;
		}
		zone = zone->next;
	}

	pthread_mutex_unlock(&g_malloc.lock);
}
